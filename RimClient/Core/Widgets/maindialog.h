/*!
 *  @brief     主面板界面
 *  @details   显示登录用户的详细信息，包括联系人、群、历史聊天对象等
 *  @file      maindialog.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.14
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180130:wey:
 *      20180222:LYS:添加主窗口面板在桌面自动贴边隐藏与弹出显示功能;
 *      20180227:wey:修复主面板在多显示屏下不能自动切换问题；修复自动隐藏在多显示屏下错误问题；
 *      20180727:LYS:修改自动贴边隐藏逻辑;
 *      20180730:LYS:修改窗口配置记录（考虑面板阴影边框）;
 *      20180730:LYS:修复主面板显示菜单时自动贴边隐藏问题;
 */
#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "widget.h"
#include "observer.h"

#include "../protocol/protocoldata.h"
using namespace ProtocolType;

class MainDialogPrivate;
class ToolItem;

class MainDialog : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainDialog)
public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

    static MainDialog * instance();

    void onMessage(MessageType type);
    void setLogInState(OnlineStatus state);
    void blockAutoHidePanel();
    void unblockAutoHidePanel();

protected:
    void resizeEvent(QResizeEvent * );
    void closeEvent(QCloseEvent * event);
    void leaveEvent(QEvent *event);
    void enterEvent(QEvent *event);

private slots:
    void updateWidgetGeometry();
    void closeWindow();
    void makeWindowFront(bool flag);
    void respSettingChange();

    void showHoverItem(bool,ToolItem*);

    void showPersonalEditWindow();
    void updateEditInstance();

    void errorGroupingOperate(OperateGrouping type);
    void screenChanged();

    //716
    void procRecvText(TextRequest response);
    void procRecvServerTextReply(TextReply);
    void procRecvFile(FileRecvDesc);

    void respOpenPanel();
    void respMenuToShow();
    void respMenuToHide();

private:
    void initWidget();
    void readSettings();
    void writeSettings();
    void initSqlDatabase();
    void initMedia();
    void isAutoHide();
    void hidePanel();
    void showPanel();

    void changeGeometry(int x,int y,int w,int h);
    void changeGeometry(QRect rect);
    void moveToDesktop(int direction);

private:
    static MainDialog * dialog;
    MainDialogPrivate * d_ptr;
};

#endif // MAINDIALOG_H
