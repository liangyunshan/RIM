/*!
 *  @brief     聊天信息记录抽象窗口
 *  @details   抽象了个人、群聊窗口中聊天信息内容显示窗口特点
 *  @file      abstractchatmainwidget.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2018.05.10
 *  @warning   嵌入html显示聊天信息。
 *  @copyright NanJing RenGu.
 *  @note
 *      20180511:LYS:抽象化聊天窗口未完成
 */
#ifndef ABSTRACTCHATMAINWIDGET_H
#define ABSTRACTCHATMAINWIDGET_H

#include <QWidget>

#include "observer.h"
#include "protocoldata.h"

struct ChatInfoUnit;
class RToolButton;
using namespace ProtocolType;

class AbstractChatMainWidgetPrivate;

class AbstractChatMainWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractChatMainWidget)
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
    enum ChatType
    {
        C2C,        //单聊模式
        GROUP       //群聊模式
    };
    explicit AbstractChatMainWidget(QWidget *parent = 0);
    ~AbstractChatMainWidget();
    void onMessage(MessageType type);
    void setUserInfo(const SimpleUserInfo &info);
    void inserHtml(QUrl &htmlUrl);
    void playVoiceMessage(QString audioName);
    void setChatType(ChatType type);

signals:
    void shakeWindow();

public slots:

private slots:
    void finishLoadHTML(bool);
    void sendShakeWindow(bool);
    void screenShotReady(bool);
    void enterSend();
    void sendMsg(bool flag=true);
    void setSideVisible(bool flag=false);

    /**********字体处理**********/
    void respFontArea(bool);
    void setMsgShowMode(int);
    void setInputAreaFont(const QFont &font);
    void setInputAreaColor(const QColor &color);

    /**********录音处理**********/
    void respShowAudioArea(bool);
    void prepareSendAudio();
    void preapreCancelAudio();

    void showQueryRecord(const ChatInfoUnit &);
    void recvTextChatMsg(const TextRequest &msg);

private:
    AbstractChatMainWidgetPrivate * d_ptr;
    void setFontIconFilePath();
    void appendMsgRecord(const TextRequest &recvMsg,MsgTarget source = RECV);
    void appendMsgRecord(const ChatInfoUnit &unitMsg, MsgTarget source = RECV);
    void appendVoiceMsg(QString recordFileName,MsgTarget source = RECV);
    void appendChatNotice(QString content,NoticeType type = NONOTICE);
    void appendChatTimeNote(QDateTime content,TimeFormat format = TIME);
};

#endif // ABSTRACTCHATMAINWIDGET_H
