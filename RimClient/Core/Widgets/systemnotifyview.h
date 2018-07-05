/*!
 *  @brief     查看系统通知消息
 *  @details   可查看系统发送的好友请求以及请求结果，并提供相应的功能操作；
 *  @attention 若同时向对方发送请求，则根据处理的时机有几种情形需要考虑：
 *             1.一方先处理，另一方后处理：
 *                  A先处理请求(双方已是好友)，B还未查看系统消息[已处理]:在B查看消息时查验B的好友列表中是否存在了A，若有则更新消息状态;
 *                  A先处理请求，B已经打开消息面板[已处理]:B在处理时先检查A是否已经为好友;
 *             2.双方同时处理请求:
 *                  a)服务器端进行规避处理[已处理]:建立关联关系时，交替查询对方是否已经在好友列表中，只有未在好友列表中才处理;
 *                  b)客户端在接收确认消息时增加判断机制，确保列表中不存在对方账户信息[已处理]：检测是否包含了好友信息;
 *  @file      systemnotifyview.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.28
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note 20180418:wey:修复好友同时向对方发送请求时，请求重复处理问题；
 */
#ifndef SYSTEMNOTIFYVIEW_H
#define SYSTEMNOTIFYVIEW_H

#include "widget.h"
#include "../protocol/datastruct.h"

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
