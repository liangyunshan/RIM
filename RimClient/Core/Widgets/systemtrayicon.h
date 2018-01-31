/*!
 *  @brief     系统托盘
 *  @details   用于在系统的任务栏中显示基本快捷操作信息
 *  @file      systemtrayicon.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.13
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180126:wey:增加图标根据不同的状态删除
 */
#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#ifdef Q_CC_MSVC
#pragma execution_character_set("utf-8")
#endif

#include <QSystemTrayIcon>

class SystemTrayIconPrivate;

class QMenu;
class QAction;
class QTimer;

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemTrayIcon)
public:
    explicit SystemTrayIcon(QObject * parent = 0);
    static SystemTrayIcon * instance();
    ~SystemTrayIcon();

    /*!
     *  @brief 托盘模式
     */
    enum SystemTrayModel
    {
        System_Login,       //登录页面模式
        System_Main         //主页面模式
    };

    enum NotifyModel
    {
        NoneNotify,
        SystemNotify,       //系统通知
        UserNotify          //用户通知
    };

    void setModel(SystemTrayIcon::SystemTrayModel model);

    void notify(NotifyModel model,QString id = QString(""),QString imagePath = QString(""));
    void removeNotify(QString id);

    void startBliking(int interval = 400);
    void stopBliking();

    SystemTrayIcon::SystemTrayModel model();

signals:
    void lockPanel();
    void showMainPanel();
    void quitApp();

private slots:
    void respIconActivated(QSystemTrayIcon::ActivationReason reason);
    void switchNotifyImage();

private:
    struct NotifyDesc
    {
        QString id;
        QString imagePath;
    };

private:
    static SystemTrayIcon * systemIcon;
    SystemTrayIconPrivate * d_ptr;
};

#endif // SYSTEMTRAYICON_H
