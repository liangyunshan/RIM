/*!
 *  @brief     文件消息接收处理
 *  @details   处理文件服务器反馈数据，根据协议解析字段，封装后交由上层处理
 *  @file      filereceiveproctask.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILERECEIVEPROCTASK_H
#define FILERECEIVEPROCTASK_H

#include "Network/rtask.h"

class FileReceiveProcTask : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    explicit FileReceiveProcTask(QObject *parent = Q_NULLPTR);
    ~FileReceiveProcTask();

    void startMe();
    void stopMe();

protected:
    void run();

private:
    void validateRecvData(const RecvUnit &data);
};

#endif // FILERECEIVEPROCTASK_H
