/*!
 *  @brief     登陆欢迎页面
 *  @details   用于读取本地配置文件方式欢迎启动页面
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.04
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180621:wey:调整登陆页面支持读取网络配置参数
 */
#ifndef SPLASHLOGINDIALOG_H
#define SPLASHLOGINDIALOG_H

#ifdef __LOCAL_CONTACT__

#include "widget.h"

#include "observer.h"
#include "datastruct.h"

class SplashLoginDialogPrivate;

class SplashLoginDialog : public Widget, public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SplashLoginDialog)
public:
    explicit SplashLoginDialog(QWidget * parent = 0);
    ~SplashLoginDialog();

    void onMessage(MessageType type);

protected:
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void initResource();
    void viewSystemNotify(NotifyInfo info,int notifyCount);
    void openChatDialog(QString accountId);
    void showNetSettings();
    void minsize();
    void closeWindow();
    void prepareNetConnect();
    void loadLocalSettings();

    /**********网络连接**********/
    void respTextConnect(bool flag);
    void respTextSocketError();
    void respTransmitError(QString errorMsg);

private:
    void enableInput(bool flag);

private:
    SplashLoginDialogPrivate * d_ptr;
};

#endif

#endif // SPLASHLOGINDIALOG_H
