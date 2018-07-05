/*!
 *  @brief     文件处理任务
 *  @details   处理用户上传或下载文件请求
 *  @file      imagetask.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.02.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILERECVTASK_H
#define FILERECVTASK_H

#include <QQueue>
#include <QWaitCondition>
#include <QMutex>

#include "Network/rtask.h"

#include "../protocol/protocoldata.h"
using namespace ProtocolType;

class FileRecvTask : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    FileRecvTask();
    ~FileRecvTask();

    enum WorkMode
    {
        FileRequestMode,
        FileTransMode
    };

    static FileRecvTask * instance();

    bool addSendItem(FileItemDesc * item);
    bool addRecvItem(SimpleFileItemRequest *item);

    void sendControlItem(SimpleFileItemRequest *item);

    void startMe();
    void stopMe();

    bool containsTask(QString fileId);
    void transfer(QString fileId);
    void nextFile();

protected:
    void run();

private:
    void handleItem();
    void transferFile();

private:
    static FileRecvTask * imageTask;

    WorkMode workMode;
    QString currTransFileId;
    FileItemDesc *  currTransFile;                      /*!< 当前传输文件 */
    QQueue<FileItemDesc *> sendItems;                   /*!< 文件发送队列描述符 */
    QQueue<SimpleFileItemRequest *> recvItems;          /*!< 文件接收队列 */
    QWaitCondition waitCondition;
    QMutex mutex;
    bool isFileTransfer;
};

#endif // IMAGETASK_H
