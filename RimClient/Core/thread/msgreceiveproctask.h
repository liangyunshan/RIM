/*!
 *  @brief     接收信息处理线程
 *  @details   处理网络接收队列中的数据，根据消息类型，预处理后将信息发送至对应的处理界面
 *  @file      msgreceiveprocthread.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.05
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGRECEIVEPROCTASK_H
#define MSGRECEIVEPROCTASK_H

#include "Network/rtask.h"
#include "protocoldata.h"
using namespace ProtocolType;

#include <QJsonObject>
#include <QJsonParseError>

namespace ClientNetwork{
class RTask;
}

class MsgReceiveProcTask : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    explicit MsgReceiveProcTask(QObject *parent = Q_NULLPTR);
    ~MsgReceiveProcTask();

    void startMe();
    void stopMe();

protected:
    void run();

private:
    void validateRecvData(const QByteArray & data);
    void handleCommandMsg(MsgCommand commandType, QJsonObject &obj);
    void handleTextMsg(MsgCommand commandType, QJsonObject &obj);

private:
    QJsonParseError jsonParseError;
};

#endif // MSGRECEIVEPROCTASK_H
