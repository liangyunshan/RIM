/*!
 *  @brief     文件消息接收处理
 *  @details   处理文件服务器反馈数据。
 *  @file      filereceiveproctask.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILERECEIVEPROCTASK_H
#define FILERECEIVEPROCTASK_H

#include <QJsonParseError>
#include <QJsonObject>

#include "Network/rtask.h"
#include "Util/rbuffer.h"
#include "protocoldata.h"
using namespace ProtocolType;

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
    void validateRecvData(const QByteArray &data);
    void handleFileMsg(MsgCommand commandType, RBuffer &obj);

private:
    QJsonParseError jsonParseError;
};

#endif // FILERECEIVEPROCTASK_H
