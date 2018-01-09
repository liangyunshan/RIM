/*!
 *  @brief     文本信息接收
 *  @details   接受客户端的文本信息、命令信息
 *  @file      msgreceive.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.08
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGRECEIVE_H
#define MSGRECEIVE_H

#include <QObject>
#include "network_global.h"

class QUdpSocket;

class NETWORKSHARED_EXPORT MsgReceive : public QObject
{
    Q_OBJECT
public:
    explicit MsgReceive(QObject *parent = 0);
    ~MsgReceive();

private:
    QString errorString;
};

#endif // MSGRECEIVE_H
