/*!
 *  @brief     登录页面
 *  @details   1.支持读取本地用户信息;2.提供登录账号选择;3.支持记住密码、自动登录; 4.支持登录验证
 *  @file      logindialog.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20171212:wey:完善登录页面，增加页面移动、读取保存用户信息；
 *             20180118:wey:取消UI界面，手写实现；
 *             20180202:wey:添加同意请求页面打开聊天页面；
 *             20180210:LYS:登录成功后将登录状态传递给面板；
 *             20180207:wey:调整用户登陆列表
 *             20180225:wey:修复切换用户其登陆状态未同步切换问题
 *             20180301:wey:修复接收好友状态信息后未及时更新缓存信息
 *             20180410:wey:修复好友列表初始不能获取登陆信息问题；调整输入验证，未登录则自动切换为默认头像
 *             20180419:wey:调整将登陆成功的账户再次登陆时置顶显示；
 */
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "widget.h"
#include "observer.h"
#include "../protocol/protocoldata.h"
using namespace ProtocolType;

class LoginDialogPrivate;
class OnLineState;
class SystemTrayIcon;
class MainDialog;
class ActionManager;

class LoginDialog : public Widget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LoginDialog)

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

    void onMessage(MessageType type);

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject * obj, QEvent *event);
    void keyPressEvent(QKeyEvent * event);

private slots:
    void login();
    void minsize();
    void closeWindow();
    void setPassword(bool flag);
    void readLocalUser();
    void validateInput();
    void validateUserName(QString name);
    void validatePassword(QString);
    void showNetSettings();
    void removeUserItem(QString accountId);
    void respItemChanged(QString id);
    void showRegistDialog();

    void respRegistDialogDestory(QObject *);
    void recvLoginResponse(ResponseLogin status,LoginResponse response);
    void recvFriendResponse(OperateFriendResponse resp);
    void viewSystemNotify(NotifyInfo info, int notifyCount);
    void openChatDialog(QString accountId);

    /**********网络连接**********/
    void respTextConnect(bool flag);
    void respTextSocketError();
    void respFileConnect(bool flag);
    void respFileSocketError();

    /**********文本消息**********/
    void procRecvText(TextRequest response);
    void procRecvServerTextReply(TextReply reply);

    /**********文件消息**********/
    void procUploadFileRequest(SimpleFileItemRequest request);
    void procDownloadFileRequest(FileItemRequest response);
    void procDownloadFileOver(QString fileId,QString fileName);

private:
    void createTrayMenu();
    void loadLocalSettings();
    int isContainUser();
    void resetDefaultInput();
    void resetDefaultPixmap();
    void enableInput(bool flag);

private:
    LoginDialogPrivate * d_ptr;
};

#endif // LOGINDIALOG_H
