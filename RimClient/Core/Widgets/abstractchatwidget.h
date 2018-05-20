/*!
 *  @brief     聊天窗口
 *  @details   抽象了个人、群聊窗口的特点
 *  @file      abstractchatwidget.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.20
 *  @warning   此窗口在关闭时，不会被销毁。只是被隐藏了。
 *  @copyright NanJing RenGu.
 */
#ifndef ABSTRACTCHATWIDGET_H
#define ABSTRACTCHATWIDGET_H

#include "widget.h"
#include "observer.h"

class AbstractChatWidgetPrivate;
class DatabaseThread;

class AbstractChatWidget : public Widget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractChatWidget)
public:
    enum MsgTarget
    {        
        SEND,   //发出信息
        RECV    //收到信息
    };
    enum NoticeType
    {
        FAULT,      //错误
        NORMAL,     //正常
        INFO,       //普通提示
        NONOTICE    //无标识
    };
    enum TimeFormat
    {
        TIME,       //时间
        DATETIME    //日期+时间
    };
    AbstractChatWidget(QWidget * parent = 0);
    ~AbstractChatWidget();

    QString widgetId();
    void recvChatMsg(QByteArray);
    void showRecentlyChatMsg(int count=1);
    void setUserInfo(SimpleUserInfo info);
    void initChatRecord();
    void onMessage(MessageType type);
    void shakeWindow();
    void appendRecvMsg(TextRequest recvMsg);
    void playVoiceMessage(QString audioName);

    void appendVoiceMsg(MsgTarget source, QString recordFileName);

public slots:
    void slot_UpdateKeySequence();
    void slot_QueryHistoryRecords(int user_query_id,int currStartRow);

private slots:
    void resizeOnce();
    void setSideVisible(bool flag);
    void slot_SetChatEditFont(bool flag);
    void slot_SetChatEditFontColor(bool flag);
    void slot_ShakeWidget(bool flag);
    void slot_ScreenShot_Ready(bool);
    void slot_ScreenShotHide(bool flag);
    void slot_ScreenTimeout();
    void slot_ButtClick_SendMsg(bool flag);
    void slot_CheckSendEnter();
    void slot_DatabaseThread_ResultReady(int,TextUnit::ChatInfoUnitList);
    void finishLoadHTML(bool);

    void noticeWebViewShift(bool);

    /**********录音处理**********/
    void respShowAudioArea(bool);
    void prepareSendAudio();
    void preapreCancelAudio();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent * event);

private:
    void switchWindowSize();
    void appendChatRecord(MsgTarget source, const TextUnit::ChatInfoUnit &unitMsg);
    void setFontIconFilePath();
    void appendChatTimeNote(QDateTime content,TimeFormat format = TIME);
    void appendChatNotice(QString content,NoticeType type = NONOTICE);

private:
    AbstractChatWidgetPrivate * d_ptr;
};

#endif // ABSTRACTCHATWIDGET_H
