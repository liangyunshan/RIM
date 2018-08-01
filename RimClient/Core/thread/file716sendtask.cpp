#include "file716sendtask.h"

#include <QFileInfo>
#include <qmath.h>
#include <QDebug>

#include "../Util/rsingleton.h"
#include "../protocol/localprotocoldata.h"
#include "../global.h"
#include "../Network/netglobal.h"
#include "../messdiapatch.h"
#include "Util/scaleswitcher.h"
#include "Network/network_global.h"

std::mutex SendMutex;
std::condition_variable SendConVariable;

#ifdef  __LOCAL_CONTACT__
std::mutex QueryNodeMutex;

/*!
 * @brief 根据节点查找对应节点的相信通信配置信息.
 * @note  [1]装备之间的通信可通过外发信息配置来约束通信方式; \n
 *        [2]指挥所内部的通信目前暂定使用TCP方式信息传输。
 * @param[in] nodeId 目的节点号
 * @param[in] result 查找结果，true表示查找成功，false表示查找失败
 * @return 所查找的节点号信息
 */
ParameterSettings::OuterNetConfig QueryNodeDescInfo(QString nodeId,bool & result){
    std::lock_guard<std::mutex> lg(QueryNodeMutex);
    result = false;

    auto findIndex = std::find_if(G_ParaSettings->outerNetConfig.begin(),G_ParaSettings->outerNetConfig.end(),
                 [&nodeId](const ParameterSettings::OuterNetConfig & config){
        if(config.nodeId == nodeId)
            return true;
        return false;
    });

    if(findIndex != G_ParaSettings->outerNetConfig.end()){
        result = true;
        return *findIndex;
    }

    if(nodeId == G_ParaSettings->baseInfo.nodeId){
        result = true;
        ParameterSettings::OuterNetConfig localConfig;
        localConfig.nodeId = nodeId;
        localConfig.communicationMethod = ParameterSettings::C_TongKong;
        localConfig.messageFormat = ParameterSettings::M_495;
        return localConfig;
    }

    return ParameterSettings::OuterNetConfig();
}

#endif

FileSendManager::FileSendManager()
{

}

bool FileSendManager::addFile(SenderFileDesc &fileInfo)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto findIndex = std::find_if(fileList.begin(),fileList.end(),[&](SenderFileDesc & fInfo){
        return fInfo == fileInfo;
    });

    if(findIndex != fileList.end()){
        return false;
    }

    fileList.push_back(fileInfo);
    SendConVariable.notify_one();

    return true;
}

/*!
 * @brief 将队列中的一个文件传输任务取消
 * @param fileInfo 文件描述信息结构体
 * @return 如果该文件存在队列中，则返回正确true
 */
bool FileSendManager::deleteFile(SenderFileDesc &fileInfo)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto findIndex = std::find_if(fileList.begin(),fileList.end(),[&](const SenderFileDesc & fdesc)->bool{
        return (fileInfo.destNodeId == fdesc.destNodeId && fileInfo.fullFilePath == fdesc.fullFilePath);
    });

    if(findIndex != fileList.end()){

        FileTransProgress progress;
        progress.transStatus = TransCancel;
        progress.fileFullPath = (*findIndex).fullFilePath;
        QFileInfo fileInfo(progress.fileFullPath);
        progress.fileName = fileInfo.fileName();
        MessDiapatch::instance()->onFileTransStatusChanged(progress);

        fileList.erase(findIndex);
        return true;
    }

    return false;
}

SenderFileDesc FileSendManager::getFile()
{
    std::unique_lock<std::mutex> ul(mutex);
    SenderFileDesc info;
    if(fileList.size() > 0){
        info = fileList.front();
        fileList.pop_front();
    }

    return info;
}

bool FileSendManager::isEmpty()
{
    std::unique_lock<std::mutex> ul(mutex);
    return !fileList.size();
}

int FileSendManager::size()
{
    std::unique_lock<std::mutex> ul(mutex);
    return fileList.size();
}

/*!
 * @brief 读取待发送的文件
 * @warning 在进行文件读写时，产生N个分包，只会在第一个分包中存在21、2051数据头，因此第一个分包的文件数据长度为512-sizeof(21)-sizeof(2051).这点较为特殊
 * @param[in/out] data 待填充的发送缓冲区
 * @return 实际读取的字节数
 */
qint64 FileSendDesc::read(QByteArray &data)
{
    if(!file.isOpen())
        return -1;

    if(isSendOver())
        return -1;

    int prepareReadLen = MAX_PACKET;
#ifdef __LOCAL_CONTACT__
    if(sliceNum == -1){
        prepareReadLen = MAX_PACKET - sizeof(QDB21::QDB21_Head) - sizeof(QDB2051::QDB2051_Head) - fileName.toLocal8Bit().length();
    }
#endif

    memset(readBuff,0,MAX_PACKET);
    qint64 realReadLen = file.read(readBuff,prepareReadLen);
    data.append(readBuff,realReadLen);
#ifdef __LOCAL_CONTACT__
    sliceNum++;
#endif
    return readLen += realReadLen;
}

#ifdef  __LOCAL_CONTACT__

File716SendTask* File716SendTask::recordTask = nullptr;

File716SendTask::File716SendTask(QObject *parent):
    ClientNetwork::RTask(parent),maxTransferFiles(2)
{
    recordTask = this;
}

File716SendTask::~File716SendTask()
{
    runningFlag = false;
    SendConVariable.notify_one();
    wait();
}

File716SendTask *File716SendTask::instance()
{
    return recordTask;
}

bool File716SendTask::addTransmit(std::shared_ptr<ClientNetwork::BaseTransmit> trans, SendCallbackFunc callback)
{
    std::lock_guard<std::mutex> lg(tranMutex);
    if(transmits.find(trans->type()) != transmits.end())
        return true;

    transmits.insert(std::pair<CommMethod,std::pair<std::shared_ptr<ClientNetwork::BaseTransmit>,SendCallbackFunc>>(trans->type(),
                             std::pair<std::shared_ptr<ClientNetwork::BaseTransmit>,SendCallbackFunc>(trans,callback)));
    return true;
}

bool File716SendTask::removaAllTransmit()
{
    std::lock_guard<std::mutex> lg(tranMutex);

    auto tbegin = transmits.begin();
    while(tbegin != transmits.end()){
        (*tbegin).second.first->close();
        (*tbegin).second.first.reset();
        tbegin = transmits.erase(tbegin);
    }

    return true;
}

void File716SendTask::startMe()
{
    RTask::startMe();

    runningFlag = true;

    if(!isRunning())
        start();
    else
        SendConVariable.notify_one();
}

void File716SendTask::stopMe()
{
    RTask::stopMe();
    runningFlag = false;
    SendConVariable.notify_one();
}

void File716SendTask::run()
{
    while(runningFlag){
        while(runningFlag && RSingleton<FileSendManager>::instance()->isEmpty()){
            std::unique_lock<std::mutex> ul(SendMutex);
            SendConVariable.wait(ul);
        }
        if(runningFlag){
            processFileData();
        }
    }
}

void File716SendTask::prepareSendTask()
{
    while(sendList.size() < maxTransferFiles){
        if(RSingleton<FileSendManager>::instance()->isEmpty()){
           break;
        }

        SenderFileDesc fileInfo = RSingleton<FileSendManager>::instance()->getFile();
        if(fileInfo.isValid()){
            std::shared_ptr<FileSendDesc> ptr = std::make_shared<FileSendDesc>();
            if(ptr->parse(fileInfo.fullFilePath) && ptr->open()){
                ptr->accountId = fileInfo.srcNodeId;
                ptr->otherSideId = fileInfo.destNodeId;
                ptr->fileType = QDB2051::F_BINARY;
                ptr->usSerialNo = fileInfo.serialNo.toUShort();
                bool result = false;
                ParameterSettings::OuterNetConfig config = QueryNodeDescInfo(fileInfo.destNodeId,result);
                if(result){
                    ptr->method = config.communicationMethod;
                    ptr->format = config.messageFormat;
                }

                sendList.push_back(ptr);
            }
        }
    }
}

/*!
 * @brief 文件数据块发送
 * @note  文件控制命令由FileSender发送 \n
 *        若需要同时发送多个文件，每次循环读取每个文件的一部分，交由网络模块直接发送。 \n
 *        根据发送成功与否，更新发送的进度。
 */
void File716SendTask::processFileData()
{
    while(true && runningFlag){
        std::list<std::shared_ptr<FileSendDesc>>::iterator iter = sendList.begin();

        FileTransProgress progress;
        progress.transType = TRANS_SEND;

        while(iter != sendList.end()){
            if((*iter)->isSendOver()){
                qDebug()<<"Send over:"<<(*iter)->fileName;

                progress.fileFullPath = (*iter)->fullPath ;
                progress.totleBytes = (*iter)->dwPackAllLen;
                progress.transStatus = TransSuccess;
                MessDiapatch::instance()->onFileTransStatusChanged(progress);

                (*iter).reset();
                iter = sendList.erase(iter);
            }else{
                SendUnit unit;
                unit.dataUnit.wSourceAddr = ScaleSwitcher::fromHexToDec((*iter)->accountId);
                unit.dataUnit.wDestAddr = ScaleSwitcher::fromHexToDec((*iter)->otherSideId);
                (*iter)->read(unit.dataUnit.data);
                unit.dataUnit.bPackType = T_DATA_AFFIRM;
                unit.dataUnit.bPeserve = 0;
                unit.dataUnit.usSerialNo = (*iter)->usSerialNo;
                unit.dataUnit.usOrderNo = O_2051;
                unit.dataUnit.cDate = 0;
                unit.dataUnit.cTime = 0;
                unit.dataUnit.cFileType = QDB2051::F_BINARY;
                unit.dataUnit.cFilename = (*iter)->fileName.toLocal8Bit();
                unit.dataUnit.dwPackAllLen = (*iter)->dwPackAllLen;
                unit.dataUnit.wOffset = (*iter)->sliceNum;

                progress.readySendBytes = (*iter)->readLen;
                progress.fileFullPath = (*iter)->fullPath ;
                progress.totleBytes = (*iter)->dwPackAllLen;

                if((*iter)->method == ParameterSettings::C_NetWork && (*iter)->format == ParameterSettings::M_205){
                    unit.method = C_UDP;
                }else if((*iter)->method == ParameterSettings::C_TongKong && (*iter)->format == ParameterSettings::M_495){
                    unit.method = C_TCP;
                }

                bool error = false;
                do{
                    if(unit.method != C_NONE){
                        auto iter = transmits.find(unit.method);
                        if(iter != transmits.end()){
                            if((*iter).second.first.get() != nullptr && (*iter).second.first->connected()){
                                if((*iter).second.first->startTransmit(unit,(*iter).second.second)){
                                    if(unit.dataUnit.wOffset == 0)
                                        progress.transStatus = TransStart;
                                    else
                                        progress.transStatus = TransProcess;
                                }else{
                                    progress.transStatus = TransError;
                                    error = true;
                                    break;
                                }
                            }else{
                                progress.transStatus = TransError;
                                error = true;
                                break;
                            }
                            MessDiapatch::instance()->onFileTransStatusChanged(progress);
                        }
                    }
                }while(0);

                //文件发送失败
                if(error){
                    (*iter).reset();
                    iter = sendList.erase(iter);
                }else{
                    iter++;
                }
            }
        }

        if(sendList.size() < maxTransferFiles){
            prepareSendTask();
        }

        if(sendList.size() == 0)
            break;
    }
}

/*!
 * @brief 计算一次完整的文件发送过程中，需要传输的数据长度(文件+各个协议头(495、21、2051))
 * @note 每包数据由：[Sizeof(495) + Sizeof(21) + Sizeof(2051) + fileNameLen] + Data 组成 \n
 *       而分包的次数为：fileSize / MAX_PACKET \n
 *       则共数据大小为：分包次数*每包数据头+fileSize
 * @param[in] fileSize 文件长度
 * @return 总数据长度
 */
unsigned long File716SendTask::countPackAllLen(int fileSize, int fileNameLen)
{
    int slice = qCeil((float)fileSize / MAX_PACKET);
    int perHeadLen = sizeof(QDB495::QDB495_SendPackage)+sizeof(QDB21::QDB21_Head)+sizeof(QDB2051::QDB2051_Head)+fileNameLen;
    unsigned long total = slice * perHeadLen + fileSize;
    return total;
}

#endif
