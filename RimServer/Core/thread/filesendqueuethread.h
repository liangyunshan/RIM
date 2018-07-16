/*!
 *  @brief     文件发送线程
 *  @details   V1.0:统一负责文件数据传输，将待传输的文件加入发送队列，发送线程轮询对每个文件发送。
 *  @author    wey
 *  @version   1.0
 *  @date      2018.07.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILESENDQUEUETHREAD_H
#define FILESENDQUEUETHREAD_H

#ifdef __LOCAL_CONTACT__

#include <QString>
#include <map>
#include <mutex>
#include <list>
#include <memory>

class QFile;

#include "../protocol/datastruct.h"
#include "Network/connection/tcpclient.h"
#include "Network/multitransmits/basetransmit.h"

/*!
 *  @brief  待发送文件管理器
 *  @note 接收所有准备发送的文件，并按照客户端节点的方式进行保存，每个节点的待发送文件均保存在queue之中 \n
 *
 */
class FileSendManager
{
public:
    explicit FileSendManager();

    bool addFile(ParameterSettings::Simple716FileInfo & fileInfo);
    ParameterSettings::Simple716FileInfo getFile();
    bool isEmpty();
    int size();

private:
    std::list<ParameterSettings::Simple716FileInfo> fileList;
    std::mutex mutex;
};

#include <QThread>
class Database;

class FileSendQueueThread : public QThread
{
    Q_OBJECT
public:
    explicit FileSendQueueThread();
    ~FileSendQueueThread();

    void startMe();

protected:
    void run();

private:
    void prepareSendTask();
    void processFileData();

private:
    /*!
     * @brief 文件发送任务描述
     */
    struct FileSendTask{
        int socketId;                                    /*!< 接收端socket连接句柄 */
        ParameterSettings::CommucationMethod method;     /*!< 通信方式 */
        ParameterSettings::MessageFormat format;         /*!< 报文格式 */
        std::shared_ptr<ServerNetwork::FileSendDesc> fptr;      /*!< 文件发送描述 */
    };

    void initTransmits();
    bool handleDataSend(SendUnit &unit);

private:
    int maxTransferClients;             /*!< 最大传输的客户端连接数 */
    bool runningFlag;
    std::list<FileSendTask> sendList;
    Database * database;

    std::map<CommMethod,BaseTransmit_Ptr> transmits;
};

#endif

#endif // FILESENDQUEUETHREAD_H
