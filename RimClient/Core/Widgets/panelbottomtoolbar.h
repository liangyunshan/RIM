/*!
 *  @brief     主面板底部菜单栏
 *  @details
 *  @file      panelbottomtoolbar.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.14
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note   20180503:wey:增加文件服务器状态显示、断网重连。
 */

#ifndef PANELBOTTOMTOOLBAR_H
#define PANELBOTTOMTOOLBAR_H

#include <QWidget>

#include "observer.h"

class QToolButton;
class PanelBottomToolBarPrivate;

class PanelBottomToolBar : public  QWidget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelBottomToolBar)
public:
    explicit PanelBottomToolBar(QWidget * parent = 0);
    ~PanelBottomToolBar();

    void onMessage(MessageType mtype);

signals:
    void showSearchPerson();

private slots:
    void showAddFriendPanel();
    void showSystemSetting();
    void showNetsettings();
    void updateFrinedInstance(QObject *);
    void updateSettingInstance(QObject *);
    void updateNetSettingInstance(QObject *);
    void showNotifyWindow();
    void updateNetConnector();
    void viewFileServerState();
    void updateNetConnectorInfo();
    void respChangeConnector(bool);

private:
    void networkIsConnected(bool connected);
    void updateNetworkState();

private:
    PanelBottomToolBarPrivate * d_ptr;
};

#endif // PANELBOTTOMTOOLBAR_H
