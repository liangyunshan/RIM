#include "rbutton.h"

#include <QFontMetrics>

#include "head.h"
#include "../../protocol/datastruct.h"
#include "../../constants.h"

#define BUTTON_TEXT_EXTRA_WIDTH     10          //宽度扩展像素
#define BUTTON_HEIGHT_SCALE_FACTOR  2           //高度倍数因子
#define BUTTON_MAX_HEIGHT 24                    //最大高度

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

    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

void RButton::setText(const QString &text)
{
    int tWidth = fontMetrics().width(text) + BUTTON_TEXT_EXTRA_WIDTH;
    setMinimumWidth(tWidth < minimumWidth()?minimumWidth():tWidth);
    int countHeight = fontMetrics().height() * BUTTON_HEIGHT_SCALE_FACTOR;
    setMinimumHeight(countHeight > BUTTON_MAX_HEIGHT ?BUTTON_MAX_HEIGHT:countHeight);

    QPushButton::setText(text);
}
