#include "filesendqueuethread.h"

#include "../rsingleton.h"
#include "../sql/databasemanager.h"
#include "../sql/sqlprocess.h"

#include <condition_variable>

std::mutex SendMutex;
std::condition_variable SendConVariable;

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

FileSendQueueThread::FileSendQueueThread():maxTransferClients(10),database(nullptr)
{

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
 */
void FileSendQueueThread::processFileData()
{
    while(true && runningFlag){
        std::list<FileSendTask>::iterator iter = sendList.begin();
        while(iter != sendList.end()){

            //TODO 20180709 发送文件
        }

        if(sendList.size() < maxTransferClients){
            prepareSendTask();
        }

        if(sendList.size() == 0)
            break;
    }
}


/*!
 * @brief 从缓存队列中按照顺序提取待发送的文件，获取待发送文件的详细信息。
 * @note  默认同一时刻通过 @link maxTransferClients @endlink 控制最大发送连接数。 \n
 *        当发送队列中存在文件被发送完成后，主动将文件移除队列，并从缓存队列中寻找是否存在新的发送任务。 \n
 */
void FileSendQueueThread::prepareSendTask()
{
    while(sendList.size() < maxTransferClients){
        if(RSingleton<FileSendManager>::instance()->isEmpty()){
           break;
        }

        if(database == nullptr)
            database = RSingleton<DatabaseManager>::instance()->newDatabase();

        ParameterSettings::Simple716FileInfo fileInfo = RSingleton<FileSendManager>::instance()->getFile();
        if(fileInfo.isValid()){
            std::shared_ptr<ServerNetwork::FileSendDesc> ptr = std::make_shared<ServerNetwork::FileSendDesc>();
            if(RSingleton<SQLProcess>::instance()->get716File(database,fileInfo.fileId,ptr)){
                FileSendTask task;
                task.socketId = fileInfo.sockId;
                task.fptr = ptr;
                sendList.push_back(task);
            }
        }
    }
}
