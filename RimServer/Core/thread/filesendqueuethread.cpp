#include "filesendqueuethread.h"

#ifdef __LOCAL_CONTACT__

#include "../rsingleton.h"
#include "../sql/databasemanager.h"
#include "../sql/sqlprocess.h"
#include "Network/netglobal.h"'
#include "Network/multitransmits/tcptransmit.h"
#include "../protocol/datastruct.h"
#include <QDebug>

#include <condition_variable>

std::mutex SendMutex;
std::condition_variable SendConVariable;

using namespace ParameterSettings;
extern OuterNetConfig QueryNodeDescInfo(unsigned short nodeId,bool & result);

FileSendManager::FileSendManager()
{

}

bool FileSendManager::addFile(ParameterSettings::Simple716FileInfo &fileInfo)
{
    std::unique_lock<std::mutex> ul(mutex);

    auto findIndex = std::find_if(fileList.begin(),fileList.end(),[&](ParameterSettings::Simple716FileInfo & fInfo){
        return fInfo == fileInfo;
    });

    if(findIndex != fileList.end()){
        return false;
    }

    fileList.push_back(fileInfo);
    SendConVariable.notify_one();

    return true;
}

ParameterSettings::Simple716FileInfo FileSendManager::getFile()
{
    std::unique_lock<std::mutex> ul(mutex);
    ParameterSettings::Simple716FileInfo info;
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

FileSendQueueThread::FileSendQueueThread():maxTransferClients(10),runningFlag(true),database(nullptr)
{
    initTransmits();
}

FileSendQueueThread::~FileSendQueueThread()
{
    runningFlag = false;
    wait();
}

void FileSendQueueThread::startMe()
{
    if(!isRunning()){
        start();
    }else{
        SendConVariable.notify_one();
    }
}

void FileSendQueueThread::run()
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


/*!
 * @brief 网络发送文件数据
 * @note  服务器读取的文件数据直接发送，不缓存发送
 */
void FileSendQueueThread::processFileData()
{
    while(true && runningFlag){
        std::list<FileSendTask>::iterator iter = sendList.begin();
        while(iter != sendList.end()){
            std::shared_ptr<ServerNetwork::FileSendDesc> fptr = (*iter).fptr;
            if(fptr->isSendOver()){
                qDebug()<<"[1]File Send over:"<<(*iter).fptr->fileName;
                fptr.reset();
                iter = sendList.erase(iter);
            }else{
                SendUnit unit;
                unit.sockId = (*iter).socketId;
                unit.dataUnit.wSourceAddr = fptr->accountId.toInt();
                unit.dataUnit.wDestAddr = fptr->otherId.toInt();
                fptr->read(unit.dataUnit.data);
                unit.dataUnit.bPackType = T_DATA_AFFIRM;
                unit.dataUnit.bPeserve = 0;
                unit.dataUnit.usSerialNo = fptr->usSerialNo;
                unit.dataUnit.usOrderNo = O_2051;
                unit.dataUnit.cDate = 0;
                unit.dataUnit.cTime = 0;
                unit.dataUnit.cFileType = QDB2051::F_BINARY;
                unit.dataUnit.cFilename = fptr->fileName.toLocal8Bit();
                unit.dataUnit.dwPackAllLen = fptr->dwPackAllLen;
                unit.dataUnit.wOffset = fptr->sliceNum;

                if((*iter).method == ParameterSettings::C_NetWork && (*iter).format == ParameterSettings::M_205){
                    unit.method = C_UDP;
                }else if((*iter).method == ParameterSettings::C_TongKong && (*iter).format == ParameterSettings::M_495){
                    unit.method = C_TCP;
                }

                if(unit.method != C_NONE){
                    if(!handleDataSend(unit)){
                        //TODO 20180713 对错误处理
                    }
                }

                iter++;
            }
         }

         if(sendList.size() < maxTransferClients){
             prepareSendTask();
         }

         if(sendList.size() == 0)
             break;
    }
}

void FileSendQueueThread::initTransmits()
{
    std::shared_ptr<ServerNetwork::TcpTransmit> tcpTrans = std::make_shared<ServerNetwork::TcpTransmit>();
    transmits.insert(std::make_pair<CommMethod,BaseTransmit_Ptr>(tcpTrans->type(),tcpTrans));
}

bool FileSendQueueThread::handleDataSend(SendUnit &unit)
{
    auto selectedTrans = transmits.find(unit.method);
    if( selectedTrans == transmits.end())
        return false;

    return (*selectedTrans).second->startTransmit(unit);
}


/*!
 * @brief 从缓存队列中按照顺序提取待发送的文件，获取待发送文件的详细信息。
 * @note  默认同一时刻通过 @link maxTransferClients @endlink 控制最大发送连接数。 \n
 *        当发送队列中存在文件被发送完成后，主动将文件移除队列，并从缓存队列中寻找是否存在新的发送任务。 \n
 */
void FileSendQueueThread::prepareSendTask()
{
    while(sendList.size() < maxTransferClients)
    {
        if(RSingleton<FileSendManager>::instance()->isEmpty()){
           break;
        }

        if(database == nullptr)
            database = RSingleton<DatabaseManager>::instance()->newDatabase();

        ParameterSettings::Simple716FileInfo fileInfo = RSingleton<FileSendManager>::instance()->getFile();
        if(fileInfo.isValid()){
            std::shared_ptr<ServerNetwork::FileSendDesc> ptr = std::make_shared<ServerNetwork::FileSendDesc>();
            if(RSingleton<SQLProcess>::instance()->get716File(database,fileInfo.fileId,ptr)){
                if(ptr->open()){
                    FileSendTask task;
                    task.socketId = fileInfo.sockId;
                    task.fptr = ptr;
                    bool result = false;
                    OuterNetConfig config = QueryNodeDescInfo(ptr->otherId.toUShort(),result);
                    if(result){
                        task.format = config.messageFormat;
                        task.method = config.communicationMethod;
                    }
                    sendList.push_back(task);
                }
            }
        }
    }
}

#endif
