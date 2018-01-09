/*!
 *  @brief     文本信息接收
 *  @details   接收文本信息，接收后直接保存由其它程序处理
 *  @file      msgreceive.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.06
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

    bool init(QString address,quint16 port);

    void stop();

private slots:
    void processData();

private:
    QUdpSocket * socket;

    QString errorString;

};

#endif // MSGRECEIVE_H
