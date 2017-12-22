#include "rbutton.h"

#include <QFontMetrics>

#include "head.h"
#include "datastruct.h"
#include "constants.h"

#define BUTTON_TEXT_EXTRA_WIDTH     10          //宽度扩展像素
#define BUTTON_HEIGHT_SCALE_FACTOR  2           //高度倍数因子

class RButtonPrivate : public GlobalData<RButton>
{
    Q_DECLARE_PUBLIC(RButton)

private:
    RButtonPrivate(RButton *q):q_ptr(q)
    {

    }
    RButton * q_ptr;

};

RButton::RButton(QWidget *parent):
    d_ptr(new RButtonPrivate(this)),
    QPushButton(parent)
{
    setMinimumWidth(70);
}

void RButton::setText(const QString &text)
{
    int tWidth = fontMetrics().width(text) + BUTTON_TEXT_EXTRA_WIDTH;
    setMinimumWidth(tWidth < minimumWidth()?minimumWidth():tWidth);
    setMinimumHeight(fontMetrics().height() * BUTTON_HEIGHT_SCALE_FACTOR);

    QPushButton::setText(text);
}
