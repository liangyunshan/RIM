/*!
 *  @brief     无边框窗口基类
 *  @details   用于实现通用功能，如边框阴影
 *  @file      abstractwidget.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      此窗口设置了一定的边距，用于绘制渐变的背景。
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include "toolbar.h"

#define WINDOW_MARGIN_SIZE 5             //边距宽度
#define WINDOW_MARGIN_WIDTH 10           //渐变背景宽度
#define WINDOW_CURSOR_DETECT_SIZE  8     //鼠标移动在边框移动时检测的范围,最好WINDOW_MARGIN_SIZE与此值一致
#define ABSTRACT_TOOL_BAR_HEGIHT 30      //工具栏固定高度

class WidgetPrivate;

class Widget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Widget)
public:
    explicit Widget(QWidget * parent);
    ~Widget();

    void setShadowWindow(bool flag = true);

protected:
    /*!
     * @brief 将窗口加入至主窗口区域
     * @param[in] child 待加入的窗口
     * @return 无
     */
    void setContentWidget(QWidget * child);

    /*!
     * @brief 是否需要开启工具栏
     * @param[in] falg 控制标识
     * @return ToolBar * 创建的工具栏，可进行进一步操作
     */
    ToolBar *enableToolBar(bool flag = false);

    virtual void paintEvent(QPaintEvent * event);
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    void repolish(QWidget * widget);

private slots:
    void setLayoutMargin();

    void showMaximizedWindow(bool);

private:
    WidgetPrivate * d_ptr;
};

#endif // ABSTRACTWIDGET_H
