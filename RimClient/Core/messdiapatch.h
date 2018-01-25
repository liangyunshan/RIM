/*!
 *  @brief     信息中转
 *  @details   将下层处理的结果，通过此类分发至具体的结果页面
 *  @file      messdiapatch.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MESSDIAPATCH_H
#define MESSDIAPATCH_H

#include <QObject>

#include "protocoldata.h"
using namespace ProtocolType;

class MessDiapatch : public QObject
{
    Q_OBJECT
public:
    static MessDiapatch * instance();

    void onRecvRegistResponse(ResponseRegister status,RegistResponse response);
    void onRecvLoginResponse(ResponseLogin status, LoginResponse response);
    void onRecvUpdateBaseInfoResponse(ResponseUpdateUser status, UpdateBaseInfoResponse response);
    void onRecvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response);
    void onRecvAddFriendResponse(ResponseAddFriend status);

signals:
    void recvRegistResponse(ResponseRegister status,RegistResponse response);
    void recvLoginResponse(ResponseLogin status,LoginResponse response);
    void recvUpdateBaseInfoResponse(ResponseUpdateUser status,UpdateBaseInfoResponse response);
    void recvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response);
    void recvAddFriendResponse(ResponseAddFriend);

private:
    MessDiapatch(QObject *parent = 0);
    static MessDiapatch * dispatch;
};

#endif // MESSDIAPATCH_H
