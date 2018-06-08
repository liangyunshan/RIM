/*!
 *  @brief     登陆欢迎页面
 *  @details   用于读取本地配置文件方式欢迎启动页面
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.04
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SPLASHLOGINDIALOG_H
#define SPLASHLOGINDIALOG_H

#ifdef __LOCAL_CONTACT__

#include <QSplashScreen>

#include "observer.h"
#include "datastruct.h"

class SplashLoginDialogPrivate;

class SplashLoginDialog : public QSplashScreen, public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SplashLoginDialog)
public:
    explicit SplashLoginDialog(QWidget * parent = 0);
    ~SplashLoginDialog();

    void onMessage(MessageType type);

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void initResource();
    void viewSystemNotify(NotifyInfo info,int notifyCount);
    void openChatDialog(QString accountId);

private:
    void addMessage(const QString &text);

private:
    SplashLoginDialogPrivate * d_ptr;
};

#endif

#endif // SPLASHLOGINDIALOG_H
