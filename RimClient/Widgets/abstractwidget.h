/*!
 *  @brief     无边框窗口基类
 *  @details   用于实现通用功能，如边框阴影
 *  @file      abstractwidget.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright GNU Public License.
 *  @note      此窗口设置了一定的边距，用于绘制渐变的背景。
 */

#ifndef ABSTRACTWIDGET_H
#define ABSTRACTWIDGET_H

#include <QWidget>

class AbstractWidget : public QWidget
{
public:
    explicit AbstractWidget(QWidget * parent);
    ~AbstractWidget();

protected:
    /*!
     * @brief 将窗口加入至主窗口区域
     *
     * @param[in] child 待加入的窗口
     *
     * @return 无
     *
     */
    void setContentWidget(QWidget * child);

    virtual void paintEvent(QPaintEvent * event);
    virtual bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private:
    QWidget * contentWidget;
};

#endif // ABSTRACTWIDGET_H
