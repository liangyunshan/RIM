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
    void recvChatAudio(QString fileName);
    void showRecentlyChatMsg(uint count=1);
    void recvTransFile(const FileTransProgress &grocess);
#ifdef __LOCAL_CONTACT__
    void setOuterNetConfig(const ParameterSettings::OuterNetConfig &config);
#endif

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void sendQueryRecord(const ChatInfoUnit &);
    void sendMsgStatus(ushort serialNo);
    void sendRecvedMsg(const TextRequest &msg);
    void sendFileTransProgress(const FileTransProgress &progress);
    void sendRecvedAudio(const QString &file);
    void sendMoreQueryRecord(const ChatInfoUnit & ,bool hasNext);
    void sendAllHistoryQueryRsult(const ChatInfoUnitList &);

public slots:
    void shakeWindow();
    void queryRecordReady(ChatInfoUnitList, QString);
    void queryMoreRecordReady(ChatInfoUnitList, QString);
    void queryAllHistoryReady(ChatInfoUnitList, QString);
    void autoQueryRecord();
    void updateMsgStatus(ushort,ushort);
    void showMaximizedWindow(bool);
    void respshowChat();

private:
    void switchWindowSize();
private:
    ChatPersonWidgetPrivate *d_ptr;
};

#endif // CHATPERSONWIDGET_H
