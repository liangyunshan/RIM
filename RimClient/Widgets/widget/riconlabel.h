/*!
 *  @brief     自定义圆角label
 *  @details   实现绘制圆角图形
 *  @file      riconlabel.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.21
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef RICONLABEL_H
#define RICONLABEL_H

#include <QLabel>

class RIconLabelPrivate;

class RIconLabel : public QLabel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RIconLabel)
public:
    RIconLabel(QWidget * parent = 0);

    void setCorner(bool isCorner = true);
    void setBackroundColor(QColor color);
    void setTransparency(bool flag = false);

    /*!
         * @brief 设置鼠标进入控件，是否改变鼠标的样式
         * @details 设置为true时，鼠标进入控件区域，样式修改为Qt::PointingHandCursor,离开时恢复为Qt::ArrowCursor
         * @param[in] flag 状态标识
         * @return 无
         *
         */
    void setEnterCursorChanged(bool flag = true);

    void setHoverDelay(bool flag = true);

protected:
    void paintEvent(QPaintEvent * event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

private slots:
    void timeOut();

signals:
    void mouseHover(bool flag);

private:
    RIconLabelPrivate * d_ptr;

};

#endif // RICONLABEL_H
