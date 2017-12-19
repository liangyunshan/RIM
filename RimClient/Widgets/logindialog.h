/*!
 *  @brief     登录页面
 *  @details   1.支持读取本地用户信息;2.提供登录账号选择;3.支持记住密码、自动登录; 4.支持登录验证
 *  @file      logindialog.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 *  @note      20171212:wey:完善登录页面，增加页面移动、读取保存用户信息；
 */
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class QToolButton;
class ToolBar;
class QMenu;
class QAction;

class LoginDialogPrivate;
class OnLineState;
class SystemTrayIcon;

class MainDialog;
class ActionManager;

class LoginDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(LoginDialog)

public:
    explicit LoginDialog(QWidget *parent = 0);
    ~LoginDialog();

protected:
    void resizeEvent(QResizeEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject * obj, QEvent *event);

private slots:
    void login();
    void minsize();
    void closeWindow();
    void switchUser(int index);
    void readLocalUser();
    void validateInput(QString text);

private:
    void initWidget();
    void createTrayMenu();
    int isContainUser();
    void resetDefaultInput();

private:
    Ui::LoginDialog *ui;
    LoginDialogPrivate * d_ptr;

    ToolBar * toolBar;
    OnLineState * onlineState;

    QToolButton * systemSetting;
    SystemTrayIcon * trayIcon;

    MainDialog * mainDialog;

    ActionManager *m_actionManager;
};

#endif // LOGINDIALOG_H
