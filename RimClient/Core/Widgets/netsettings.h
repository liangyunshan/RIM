/*!
 *  @brief     网路设置窗口
 *  @details   提供设置登陆服务器的基本信息
 *  @file      netsettings.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.17
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef NETSETTINGS_H
#define NETSETTINGS_H

#include "widget.h"

class QSettings;
class NetSettingsPrivate;

#include "protocoldata.h"
using namespace ProtocolType;

class NetSettings : public Widget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(NetSettings)
public:
    explicit NetSettings(QWidget * parent = 0);
    ~NetSettings();

    void initLocalSettings();

signals:
    void ipInfoUpdated();

private slots:
    void updateSettings();

private:
    NetSettingsPrivate * d_ptr;

};

#endif // NETSETTINGS_H
