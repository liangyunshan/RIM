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
 */
#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "widget.h"
#include "observer.h"

#include "protocoldata.h"
using namespace ProtocolType;

class MainDialogPrivate;
class ToolBar;
class PanelBottomToolBar;
class PanelContentArea;
class PanelTopArea;
class ToolItem;
class DatabaseManager;

class MainDialog : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainDialog)
public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

    static MainDialog * instance();

    void onMessage(MessageType type);

protected:
    void resizeEvent(QResizeEvent * );
    void closeEvent(QCloseEvent * event);

private slots:
    void updateWidgetGeometry();
    void closeWindow();
    void makeWindowFront(bool flag);

    void showChatWindow(ToolItem*item);
    void showHoverItem(bool,ToolItem*);

    void showPersonalEditWindow();
    void updateEditInstance();

private:
    void initWidget();
    void readSettings();
    void writeSettings();
    void initSqlDatabase();

private:
    static MainDialog * dialog;
    MainDialogPrivate * d_ptr;
    DatabaseManager *p_dbManager;
};

#endif // MAINDIALOG_H
