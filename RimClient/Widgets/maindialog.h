/*!
 *  @brief     主面板界面
 *  @details   显示登录用户的详细信息，包括联系人、群、历史聊天对象等
 *  @file      maindialog.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.14
 *  @warning
 *  @copyright GNU Public License.
 *  @note
 */
#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include "abstractwidget.h"

class MainDialogPrivate;
class ToolBar;
class PanelBottomToolBar;
class PanelContentArea;
class PanelTopArea;

class MainDialog : public AbstractWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainDialog)
public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);
    void resizeEvent(QResizeEvent * );
    void closeEvent(QCloseEvent * event);

private slots:
    void updateWidgetGeometry();
    void closeWindow();

private:
    void initWidget();
    void readSettings();
    void writeSettings();

private:

    MainDialogPrivate * d_ptr;

    QWidget * MainPanel;
    QWidget * TopBar;
    QWidget * Conent;
    QWidget * ToolBarWidget;

    ToolBar * toolBar;
    PanelBottomToolBar * bottomToolBar;
    PanelContentArea * panelContentArea;
    PanelTopArea * panelTopArea;
};

#endif // MAINDIALOG_H
