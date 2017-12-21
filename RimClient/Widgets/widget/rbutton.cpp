#include "rbutton.h"

#include "head.h"
#include "datastruct.h"
#include "constants.h"

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

}
