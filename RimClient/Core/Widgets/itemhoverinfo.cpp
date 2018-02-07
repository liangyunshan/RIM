#include "itemhoverinfo.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "head.h"
#include "datastruct.h"
#include "constants.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#include <WinUser.h>
#endif

#define ITEM_HOVER_FIX_WIDTH 300
#define ITEM_HOVER_FIX_HEIGHT 180

#define ITEM_ANIMATION_INTERVAL 250

class ItemHoverInfoPrivate : public GlobalData<ItemHoverInfo>
{
    Q_DECLARE_PUBLIC(ItemHoverInfo)

private:
    ItemHoverInfoPrivate(ItemHoverInfo * q):q_ptr(q)
    {
        initWidget();
        fadeinGroup = NULL;
        fadeinAnimateion = NULL;
        fadeInOpacityAnmiation = NULL;

        fadeoutGroup = NULL;
        fadeoutAnimateion = NULL;
        fadeoutOpacityAnmiation = NULL;
    }

    ~ItemHoverInfoPrivate();

    ItemHoverInfo * q_ptr;

    void initWidget();

    QWidget * contentWidget;

    QLabel * userNameLabel;
    QLabel * signNameLabel;
    QLabel * accountLabel;

    QPoint toolItemPos;

    QParallelAnimationGroup * fadeinGroup;
    QPropertyAnimation * fadeInOpacityAnmiation;
    QPropertyAnimation * fadeinAnimateion;

    QParallelAnimationGroup * fadeoutGroup;
    QPropertyAnimation * fadeoutOpacityAnmiation;
    QPropertyAnimation * fadeoutAnimateion;
};

ItemHoverInfoPrivate::~ItemHoverInfoPrivate()
{
    if(fadeinGroup)
    {
        if(fadeinGroup->state() == QAbstractAnimation::Running)
        {
            fadeinGroup->stop();
        }
        delete fadeinGroup;
    }

    if(fadeoutGroup)
    {
        if(fadeoutGroup->state() == QAbstractAnimation::Running)
        {
            fadeoutGroup->stop();
        }
        delete fadeoutGroup;
    }
}

void ItemHoverInfoPrivate::initWidget()
{
    contentWidget = new QWidget();
    contentWidget->setObjectName("AbstractWidget_ContentWidget");
    contentWidget->setStyleSheet("QWidget#AbstractWidget_ContentWidget{border-image:url(':/background/resource/background/item_hover.png')}");

    QVBoxLayout * vlayout = new QVBoxLayout();
    vlayout->setContentsMargins(15,15,0,0);
    vlayout->setSpacing(6);

    userNameLabel = new QLabel(contentWidget);
    userNameLabel->setMinimumHeight(30);
    userNameLabel->setFont(QFont("微软雅黑",15,QFont::Bold));
    userNameLabel->setStyleSheet("color:white");

    signNameLabel = new QLabel(contentWidget);
    signNameLabel->setFont(QFont("微软雅黑",13,QFont::Bold));
    signNameLabel->setStyleSheet("color:white");
    signNameLabel->setMinimumHeight(Constant::ITEM_FIX_HEIGHT);

    accountLabel = new QLabel(contentWidget);
    accountLabel->setStyleSheet("color:white");
    accountLabel->setMinimumHeight(Constant::ITEM_FIX_HEIGHT);

    vlayout->addWidget(userNameLabel);
    vlayout->addWidget(signNameLabel);
    vlayout->addWidget(accountLabel);
    vlayout->addStretch(3);

    contentWidget->setLayout(vlayout);

    q_ptr->setContentWidget(contentWidget);
}

ItemHoverInfo::ItemHoverInfo(QWidget *parent):
    d_ptr(new ItemHoverInfoPrivate(this)),
    Widget(parent)
{
    setFixedSize(ITEM_HOVER_FIX_WIDTH,ITEM_HOVER_FIX_HEIGHT);
    setAttribute(Qt::WA_DeleteOnClose,true);

#ifdef Q_OS_WIN
    ::SetWindowPos(HWND(winId()), HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#else
    setWindowFlags(Qt::WindowStaysOnTopHint);
#endif

}

ItemHoverInfo::~ItemHoverInfo()
{
    delete d_ptr;
}

void ItemHoverInfo::fadein(QPoint itemPos)
{
    MQ_D(ItemHoverInfo);
    d->toolItemPos = itemPos;
    show();
    if(!d->fadeinGroup)
    {
        d->fadeinAnimateion = new QPropertyAnimation(this,"geometry");
        d->fadeinAnimateion->setKeyValueAt(0,QRect(itemPos.x() - ITEM_HOVER_FIX_WIDTH,itemPos.y() - ITEM_HOVER_FIX_HEIGHT/4,ITEM_HOVER_FIX_WIDTH,ITEM_HOVER_FIX_HEIGHT));
        d->fadeinAnimateion->setKeyValueAt(1,QRect(itemPos.x() - ITEM_HOVER_FIX_WIDTH,itemPos.y(),ITEM_HOVER_FIX_WIDTH,ITEM_HOVER_FIX_HEIGHT));
        d->fadeinAnimateion->setDuration(ITEM_ANIMATION_INTERVAL);
        d->fadeinAnimateion->setEasingCurve(QEasingCurve::InQuad);

        d->fadeInOpacityAnmiation = new QPropertyAnimation(this,"windowOpacity");
        d->fadeInOpacityAnmiation->setKeyValueAt(0,0);
        d->fadeInOpacityAnmiation->setKeyValueAt(1,1);
        d->fadeInOpacityAnmiation->setDuration(ITEM_ANIMATION_INTERVAL);
        d->fadeInOpacityAnmiation->setEasingCurve(QEasingCurve::InQuad);

        d->fadeinGroup = new QParallelAnimationGroup();
        d->fadeinGroup->addAnimation(d->fadeinAnimateion);
        d->fadeinGroup->addAnimation(d->fadeInOpacityAnmiation);
    }
    d->fadeinGroup->start();
}

void ItemHoverInfo::fadeout()
{
    MQ_D(ItemHoverInfo);
    if(!d->fadeoutGroup)
    {
        d->fadeoutAnimateion = new QPropertyAnimation(this,"geometry");
        d->fadeoutAnimateion->setKeyValueAt(0,QRect(d->toolItemPos.x() - ITEM_HOVER_FIX_WIDTH,d->toolItemPos.y(),ITEM_HOVER_FIX_WIDTH,ITEM_HOVER_FIX_HEIGHT));
        d->fadeoutAnimateion->setKeyValueAt(1,QRect(d->toolItemPos.x() - ITEM_HOVER_FIX_WIDTH,d->toolItemPos.y() - ITEM_HOVER_FIX_HEIGHT/4,ITEM_HOVER_FIX_WIDTH,ITEM_HOVER_FIX_HEIGHT));
        d->fadeoutAnimateion->setDuration(ITEM_ANIMATION_INTERVAL);
        d->fadeoutAnimateion->setEasingCurve(QEasingCurve::InQuad);
        connect(d->fadeoutAnimateion,SIGNAL(finished()),this,SLOT(close()));

        d->fadeoutOpacityAnmiation = new QPropertyAnimation(this,"windowOpacity");
        d->fadeoutOpacityAnmiation->setKeyValueAt(0,1);
        d->fadeoutOpacityAnmiation->setKeyValueAt(1,0);
        d->fadeoutOpacityAnmiation->setDuration(ITEM_ANIMATION_INTERVAL);
        d->fadeoutOpacityAnmiation->setEasingCurve(QEasingCurve::InQuad);

        d->fadeoutGroup = new QParallelAnimationGroup();
        d->fadeoutGroup->addAnimation(d->fadeoutAnimateion);
        d->fadeoutGroup->addAnimation(d->fadeoutOpacityAnmiation);
    }
    d->fadeoutGroup->start();
}

void ItemHoverInfo::setSimpleUserInfo(SimpleUserInfo info)
{
    MQ_D(ItemHoverInfo);
    d->userNameLabel->setText(info.nickName);
    d->signNameLabel->setText(info.signName);
    d->accountLabel->setText(info.accountId);
}
