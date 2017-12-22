/*!
 *  @brief     控制聊天窗口右侧xia显示或隐藏
 *  @details
 *  @file      slidebar.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef SLIDEBAR_H
#define SLIDEBAR_H

#include <QWidget>

class SlideBarPrivate;

class SlideBar : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SlideBar)
public:
    SlideBar(QWidget * parent = 0);
    ~SlideBar();

    void setState(bool flag);

signals:
    void slideStateChanged(bool isVisible);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    SlideBarPrivate * d_ptr;

};

#endif // SLIDEBAR_H
