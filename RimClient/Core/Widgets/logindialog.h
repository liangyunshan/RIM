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
 */
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "widget.h"
#include "observer.h"
#include "protocoldata.h"
using namespace ProtocolType;

class QToolButton;
class ToolBar;
class QMenu;
class QAction;
class QLabel;
class RIconLabel;
class QPushButton;
class QListWidgetItem;

class LoginDialogPrivate;
class OnLineState;
class SystemTrayIcon;
class MainDialog;
class ActionManager;

class ComboxItem : public QWidget
{
    Q_OBJECT
public:
    explicit ComboxItem(QWidget * parent = 0);
    ~ComboxItem();

    void setNickName(QString name);

    void setAccountId(QString id);
    QString getAccountId();

protected:
    void mouseReleaseEvent(QMouseEvent *);

signals:
    void deleteItem(QString id);
    void itemClicked(QString id);

private slots:
    void prepareDelete();

private:
    RIconLabel * iconLabel;
    QLabel * nickNameLabel;
    QLabel * accountLabel;
    QPushButton * closeButt;
};

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

private slots:
    void login();
    void minsize();
    void closeWindow();
    void setPassword(bool flag);
    void readLocalUser();
    void validateInput(QString text);
    void showNetSettings();
    void removeUserItem(QString accountId);
    void respItemChanged(QString id);
    void showRegistDialog();
    void respConnect(bool flag);
    void respRegistDialogDestory(QObject *);
    void recvLoginResponse(ResponseLogin status,LoginResponse response);
    void recvFriendResponse(OperateFriendResponse resp);
    void viewSystemNotify(NotifyInfo info, int notifyCount);
    void openChatDialog(QString accountId);
    void procRecvText(TextResponse response);

private:
    void createTrayMenu();
    void loadLocalSettings();
    int isContainUser();
    void resetDefaultInput();

private:
    LoginDialogPrivate * d_ptr;
};

#endif // LOGINDIALOG_H
