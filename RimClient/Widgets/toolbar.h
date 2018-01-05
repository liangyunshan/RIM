/*!
 *  @brief     自定义工具栏
 *  @details   1.支持添加、插入工具按钮
 *             2.支持控制程序图标和窗口名称显示控制
 *
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      工具栏的组成:[ICON][TEXT][A]XXXXX[A][A][A]  [ICON]表示图标 [TEXT]表示窗口名称  [A]表示工具按钮；XXX表示弹簧
 *  @note      20171227:wey:增加窗口工具类型ToolTypeFlag,简化窗口设置
 */
#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <QWidget>

namespace Ui {
class ToolBar;
}

class QHBoxLayout;
class RToolButton;
class ToolBarPrivate;

class ToolBar : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolBar)

public:
    explicit ToolBar(QWidget *parent = 0);
    ~ToolBar();

    void setObjectName(const QString & name);
    QString objectName()const;

    void setIconSize(QSize size);
    QSize iconSize()const;

    void addStretch(int strech = 0);

    void setSpacing(int spacing);

    void setWindowIcon(const QString image);

    void setWindowTitle(const QString& title);
    QString windowTitle() const;

    void setContentsMargins(int left, int top, int right, int bottom);

    /*!
     *  @brief 工具栏类型，可动态设置不同窗口工具栏组合，将ToolBar利用最大化
     */
    enum ToolTypeFlag
    {
        TOOL_NONE = 0x0000,             //无
        TOOL_MIN = 0x0001,              //最小化按钮
        TOOL_MAX = 0x0002,              //最大化按钮
        TOOL_CLOSE = 0x0004,            //关闭按钮
        TOOL_ICON = 0x0008,             //系统图标
        TOOL_TITLE = 0x0010,            //系统文字
        TOOL_SPACER = 0x1000,           //弹簧
        TOOL_MESSAGEBOX = TOOL_ICON | TOOL_TITLE | TOOL_SPACER | TOOL_CLOSE,            //弹出窗口
        TOOL_DIALOG = TOOL_ICON | TOOL_TITLE | TOOL_MIN | TOOL_CLOSE |TOOL_SPACER,      //dialog窗口工具栏
        TOOL_ACTION = TOOL_MIN | TOOL_CLOSE | TOOL_MAX |TOOL_SPACER,                    //按钮区
        TOOL_WIDGET = TOOL_DIALOG | TOOL_MAX                                            //widget窗口工具栏
    };
    Q_DECLARE_FLAGS(ToolType,ToolTypeFlag)
    Q_FLAG(ToolType)

    void setToolFlags(ToolBar::ToolType);

    ToolBar::ToolType flags();

    bool appendToolButton(RToolButton * toolButton);

    bool insertToolButton(RToolButton * toolButton,const char * ID = NULL);

signals:
    void closeWindow();
    void minimumWindow();
    void maxWindow(bool);

private:
    ToolBarPrivate * d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ToolBar::ToolType)

#endif // TOOLBAR_H
