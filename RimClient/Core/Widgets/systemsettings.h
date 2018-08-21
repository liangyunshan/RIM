/*!
 *  @brief     系统设置页面
 *  @details   用于设置系统常用的功能
 *  @file      systemsettings.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.29
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include "widget.h"
#include "observer.h"

class QLabel;
class QGridLayout;

class SystemSettingsPage : public QWidget
{
    Q_OBJECT

public:
    SystemSettingsPage(QWidget * parent = 0);
    void setDescInfo(QString text);
    void addItem(QWidget * item);
    void addItem(QWidget * item,int row,int column,int rowSpan,int columnSpan);

    int row();

protected:
    void paintEvent(QPaintEvent * event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    QWidget * content;

    QLabel * descLabel;
    QGridLayout * mainLayout;
    int itemCount;
    int rowCount;

    bool mouseEnter;
};

class SystemSettingsPrivate;

class SystemSettings : public Widget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemSettings)
public:
    SystemSettings(QWidget * parent = 0);
    ~SystemSettings();

    void onMessage(MessageType type);

private slots:
    void respAutoStartUp(bool flag);
    void respAutoLogIn(bool flag);
    void respKeepFront(bool flag);
    void respExitSystem(bool flag);
    void respSystemTrayIcon(bool flag);
    void respHidePanel(bool flag);
    void respWindowShake(bool flag);
    void respSoundAvailable(bool flag);
    void respSystemLock(bool flag);
    void respDelRecord(bool flag);
    void respTextEncryption(bool flag);
    void respTextCompression(bool flag);

    void respAutoReply();
    void respShortCut();
    void respSoundSetting();
    void respFileSetting();
    void respQuickOrder();

private:
    SystemSettingsPrivate * d_ptr;

};

#endif // SYSTEMSETTINGS_H
