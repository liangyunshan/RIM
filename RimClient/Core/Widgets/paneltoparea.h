/*!
 *  @brief     主面板顶部信息区
 *  @details   显示登录用户的头像、名称、签名并支持以后扩展添加工具
 *  @file      paneltoparea.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180130:wey:响应用户信息更改
 *      20180207:wey:修改签名保存至数据库
 */
#ifndef PANELTOPAREA_H
#define PANELTOPAREA_H

#include <QWidget>
#include "observer.h"
#include "onlinestate.h"
#include "../protocol/protocoldata.h"

class QLabel;
class QLineEdit;
class PanelTopAreaPrivate;

using namespace ProtocolType;

class PanelTopArea : public QWidget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelTopArea)
public:
    explicit PanelTopArea(QWidget *parent = 0);
    ~PanelTopArea();

    void onMessage(MessageType type);
    void setState(OnlineStatus state);

    QMenu *loginStateMenu();

private slots:
#ifndef __LOCAL_CONTACT__
    void respSignChanged(QString content);
    void recvBaseInfoResponse(ResponseUpdateUser result,UpdateBaseInfoResponse response);
    void recvUserStateChanged(MsgOperateResponse result,UserStateResponse response);

public slots:
    void stateChanged(OnlineStatus state);

#endif

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void updateUserInfo();
    void loadCustomUserImage();
    void networkIsConnected(bool connected);

private:
    PanelTopAreaPrivate * d_ptr;
};

#endif // PANELTOPAREA_H
