/*!
 *  @brief     单聊窗口
 *  @details   C2C聊天窗口
 *  @file      chatpersonwidget.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2018.06.06
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef CHATPERSONWIDGET_H
#define CHATPERSONWIDGET_H

#include "widget.h"
#include "observer.h"

class ChatPersonWidgetPrivate;

class ChatPersonWidget : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ChatPersonWidget)
public:
    ChatPersonWidget(QWidget * parent = 0);
    ~ChatPersonWidget();
    void onMessage(MessageType type);
    void initChatRecord();
    void setUserInfo(const SimpleUserInfo &info);
    void recvChatMsg(const TextRequest &msg);
    void showRecentlyChatMsg(uint count=1);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent * event);

signals:
    void sendQueryRecord(const ChatInfoUnit &);
    void sendRecvedMsg(const TextRequest &msg);

public slots:
    void shakeWindow();
    void queryRecordReady(ChatInfoUnitList &);

private:
    void switchWindowSize();
private:
    ChatPersonWidgetPrivate *d_ptr;
};

#endif // CHATPERSONWIDGET_H
