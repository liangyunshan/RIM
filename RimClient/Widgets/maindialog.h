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

#include <QWidget>

namespace Ui {
class MainDialog;
}

class MainDialogPrivate;
class ToolBar;

class MainDialog : public QWidget
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

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private slots:
    void updateWidgetGeometry();
    void closeWindow();

private:
    void initWidget();
    void readSettings();
    void writeSettings();

    int     row(QPointF pos);                //计算九宫格行
    int     col(QPointF pos);                //计算九宫格列
    int     moveArea(QPointF pos);          //点击区域 相对于九宫格
    void    setMouseStyle(int moveArea);

private:
    Ui::MainDialog *ui;

    MainDialogPrivate * d_ptr;

    ToolBar * toolBar;
};

#endif // MAINDIALOG_H
