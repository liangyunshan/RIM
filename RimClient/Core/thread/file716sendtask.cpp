#include "file716sendtask.h"

#include <QFileInfo>
#include <qmath.h>
#include <QDebug>

#include "../Util/rsingleton.h"
#include "../protocol/localprotocoldata.h"
#include "../global.h"
#include "../Network/netglobal.h"

std::mutex SendMutex;
std::condition_variable SendConVariable;

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

void File716SendTask::processFileData()
{
    while(true && runningFlag){
        std::list<std::shared_ptr<FileSendDesc>>::iterator iter = sendList.begin();

        //TODO:发送实时传输状态给进度控制单元 FileTransProgress progress;
        int processUnit = 1;
        if(sendList.size()>=100)
        {
            processUnit = sendList.size()/100;
        }

        FileTransProgress progress;
        if(sendList.size() >0)
        {
            progress.fileFullPath = (*iter)->fullPath ;
            progress.totleBytes = (*iter)->dwPackAllLen;
        }
        while(iter != sendList.end()){
            if((*iter)->isSendOver()){
                qDebug()<<"Send over:"<<(*iter)->fileName;

                progress.transStatus = TransSuccess;
                emit sigTransStatus(progress);

                (*iter).reset();
                iter = sendList.erase(iter);
            }else{
                SendUnit unit;
                unit.dataUnit.wSourceAddr = (*iter)->accountId.toInt();
                unit.dataUnit.wDestAddr = (*iter)->otherSideId.toInt();
                (*iter)->read(unit.dataUnit.data);
                unit.dataUnit.bPackType = T_DATA_AFFIRM;
                unit.dataUnit.bPeserve = 0;
                unit.dataUnit.usSerialNo = 4567;
                unit.dataUnit.usOrderNo = O_2051;
                unit.dataUnit.cDate = 0;
                unit.dataUnit.cTime = 0;
                unit.dataUnit.cFileType = QDB2051::F_BINARY;
                unit.dataUnit.cFilename = (*iter)->fileName.toLocal8Bit();
                unit.dataUnit.dwPackAllLen = (*iter)->dwPackAllLen;
                unit.dataUnit.wOffset = (*iter)->sliceNum;

                progress.readySendBytes = (*iter)->readLen;
                if(unit.dataUnit.wOffset==0)
                {
                    progress.transStatus = TransStart;
                    emit sigTransStatus(progress);
                }
                else
                {
                    if(unit.dataUnit.wOffset%processUnit == 0)
                    {
                        progress.transStatus = TransProcess;
                        emit sigTransStatus(progress);
                    }
                }

                if((*iter)->method == ParameterSettings::C_NetWork && (*iter)->format == ParameterSettings::M_205){
                    unit.method = C_UDP;
                }else if((*iter)->method == ParameterSettings::C_TongKong && (*iter)->format == ParameterSettings::M_495){
                    unit.method = C_TCP;
                }

                if(unit.method != C_NONE){
                    G_FileSendMutex.lock();
                    G_FileSendBuffs.push(unit);
                    G_FileSendMutex.unlock();
                    G_FileSendWaitCondition.notify_one();
                }
                iter++;
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
