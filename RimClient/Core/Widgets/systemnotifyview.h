/*!
 *  @brief     查看系统通知消息
 *  @details   可查看系统发送的好友请求以及请求结果，并提供相应的功能操作；
 *  @file      systemnotifyview.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SYSTEMNOTIFYVIEW_H
#define SYSTEMNOTIFYVIEW_H

#include "widget.h"
#include "datastruct.h"

class SystemNotifyViewPrivate;

class SystemNotifyView : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemNotifyView)
public:
    explicit SystemNotifyView(QWidget * parent = 0);
    ~SystemNotifyView();

    void setNotifyType(ResponseFriendApply type);
    void setNotifyInfo(NotifyInfo info);

protected:
    void resizeEvent(QResizeEvent *);

signals:
    void chatWidth(QString accountId);

private slots:
    void respAgree();
    void respRefuse();
    void respIgnore();
    void respChat();
    void respReRequest();

private:
    void sendResponse(ResponseFriendApply result);

private:
    SystemNotifyViewPrivate * d_ptr;
};

#endif // SYSTEMNOTIFYVIEW_H
