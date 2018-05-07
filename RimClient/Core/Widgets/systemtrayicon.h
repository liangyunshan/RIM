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
 *      20180507:wey:增加移除全部通知消息
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
     *  @brief 托盘工作模式
     */
    enum SystemTrayModel
    {
        System_Login,       /*!< 登录页面模式 */
        System_Main         /*!< 主页面模式 */
    };

    /*!
     *  @brief 通知类型
     */
    enum NotifyModel
    {
        NoneNotify,         /*!< 无通知 */
        SystemNotify,       /*!< 系统通知 */
        UserNotify          /*!< 用户通知 */
    };

    void setModel(SystemTrayIcon::SystemTrayModel model);

    void notify(NotifyModel model,QString id = QString(""),QString imagePath = QString(""));
    void frontNotify(SystemTrayIcon::NotifyModel model,QString id = QString(""));
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
    void removeAll();

private:
    /*!
     *  @brief 通知具体描述信息
     *  @details 通过此描述一个通知的详细信息
     */
    struct NotifyDesc
    {
        NotifyModel model;       /*!< @link NotifyModel @endlink */
        QString id;              /*!< 与NotifyWinwos中通知id保持一致 */
        QString imagePath;       /*!< 对应图片信息 */
    };

private:
    static SystemTrayIcon * systemIcon;
    SystemTrayIconPrivate * d_ptr;
};

#endif // SYSTEMTRAYICON_H
