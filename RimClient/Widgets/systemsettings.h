/*!
 *  @brief     系统设置页面
 *  @details   用于设置系统常用的功能
 *  @file      systemsettings.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.29
 *  @warning
 *  @copyright GNU Public License.
 */

#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include "widget.h"

class QLabel;
class QGridLayout;

class SystemSettingsPage : public QWidget
{
    Q_OBJECT

public:
    SystemSettingsPage(QWidget * parent = 0);
    void setDescInfo(QString & text);
    void addItem(QWidget * item);

protected:
    void paintEvent(QPaintEvent * event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    QWidget * content;

    QLabel * descLabel;
    QGridLayout * mainLayout;
    int itemCount;

    bool mouseEnter;

};

class SystemSettingsPrivate;

class SystemSettings : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SystemSettings)
public:
    SystemSettings(QWidget * parent = 0);
    ~SystemSettings();

private:
    SystemSettingsPrivate * d_ptr;

};

#endif // SYSTEMSETTINGS_H
