/*!
 *  @brief     聊天窗口
 *  @details   抽象了个人、群聊窗口的特点
 *  @file      abstractchatwidget.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.20
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef ABSTRACTCHATWIDGET_H
#define ABSTRACTCHATWIDGET_H

#include "abstractwidget.h"

class AbstractChatWidgetPrivate;

class AbstractChatWidget : public AbstractWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractChatWidget)
public:
    AbstractChatWidget(QWidget * parent = 0);

    QString widgetId();

protected:
    void resizeEvent(QResizeEvent * event);

private:
    AbstractChatWidgetPrivate * d_ptr;
};

#endif // ABSTRACTCHATWIDGET_H
