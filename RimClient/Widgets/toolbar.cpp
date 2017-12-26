#include "toolbar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QStyle>
#include <QList>
#include <QLabel>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "actionmanager/actionmanager.h"

#define WINDOW_ICON_SIZE 16

template<class T>
bool contains(QList<T*> & list,const T * t)
{
    /*the compiler can't inspect list<tNode<T>*> (it doesn't have its definition at this point)
    and so it doesn't know whether list<tNode<T>*>::iterator is either a static field or a type. 需要加上typename*/

    typename  QList<T*>::iterator iter = list.begin();
    while(iter != list.end())
    {
        if((*iter) == t)
        {
            return true;
        }
        iter++;
    }

    return false;
}

class ToolBarPrivate : public GlobalData<ToolBar>
{
    Q_DECLARE_PUBLIC(ToolBar)

private:
    ToolBarPrivate(bool iconVisible,ToolBar * q):q_ptr(q)
    {
        iconVisilble = false;
        titleVisible = false;
        initDialog(iconVisible);
        iconSize = QSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
    }

    void initDialog(bool iconVisible);

    ToolBar * q_ptr;

    QWidget * toolBar;
    QHBoxLayout * toolBarLayout;

    bool iconVisilble;
    bool titleVisible;
    QLabel * iconLabel;
    QLabel * windowTitle;

    QSize iconSize;

    QList<RToolButton *> toolButts;
};

void ToolBarPrivate::initDialog(bool iconVisible)
{
    toolBar = new QWidget(q_ptr);
    toolBar->setMinimumHeight(Constant::TOOL_BAR_HEIGHT);

    toolBarLayout = new QHBoxLayout();

    if(iconVisible)
    {
        iconLabel = new QLabel(toolBar);
        iconLabel->setFixedSize(WINDOW_ICON_SIZE,WINDOW_ICON_SIZE);
        iconLabel->setVisible(iconVisilble);

        windowTitle = new QLabel(toolBar);
        windowTitle->setFont(QFont("微软雅黑",10));
        windowTitle->setVisible(titleVisible);

        toolBarLayout->addWidget(iconLabel);
        toolBarLayout->addSpacing(4);
        toolBarLayout->addWidget(windowTitle);
        toolBarLayout->addStretch(1);
    }

    toolBarLayout->setContentsMargins(5,0,0,0);
    toolBarLayout->setSpacing(0);
    toolBar->setLayout(toolBarLayout);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(toolBar);

    q_ptr->setLayout(layout);
}

ToolBar::ToolBar(bool iconVisible,QWidget *parent) :
    d_ptr(new ToolBarPrivate(iconVisible,this)),
    QWidget(parent)
{

}

ToolBar::~ToolBar()
{

}

void ToolBar::setIconSize(QSize size)
{
    MQ_D(ToolBar);
    d->iconSize = size;
}

void ToolBar::addStretch(int strech)
{
    MQ_D(ToolBar);
    d->toolBarLayout->addStretch(strech);
}

void ToolBar::setSpacing(int spacing)
{
    MQ_D(ToolBar);
    d->toolBarLayout->setSpacing(spacing);
}

void ToolBar::enableWindowIcon(bool flag)
{
    MQ_D(ToolBar);
    d->iconVisilble = flag;
    d->iconLabel->setVisible(flag);
}

void ToolBar::setWindowIcon(const QString image)
{
    MQ_D(ToolBar);
    if(d->iconVisilble)
    {
        d->iconLabel->setPixmap(QPixmap(image));
    }
}

void ToolBar::enableWindowTitle(bool flag)
{
    MQ_D(ToolBar);
    d->titleVisible = flag;
    d->windowTitle->setVisible(flag);
}

void ToolBar::setWindowTitle(const QString &name)
{
    MQ_D(ToolBar);
    if(d->titleVisible)
    {
        d->windowTitle->setText(name);
    }
}

QString ToolBar::windowTitle() const
{
    MQ_D(ToolBar);
    return d->windowTitle->text();
}

void ToolBar::setContentsMargins(int left, int top, int right, int bottom)
{
    MQ_D(ToolBar);
    d->toolBarLayout->setContentsMargins(left,top,right,bottom);
}

bool ToolBar::appendToolButton(RToolButton *toolButton)
{
    MQ_D(ToolBar);
    if(!contains(d->toolButts,toolButton))
    {
        toolButton->setFixedSize(d->iconSize);
        d->toolBarLayout->addWidget(toolButton);
    }
    return true;
}

bool ToolBar::insertToolButton(RToolButton *toolButton, const char *ID)
{
    MQ_D(ToolBar);
    if(!contains(d->toolButts,toolButton))
    {
        if(ID == NULL)
        {
            d->toolBarLayout->insertWidget(0,toolButton);
            return true;
        }
        else
        {
            int index = 0;
            foreach(QObject * tmpObj,d->toolBar->children())
            {
                RToolButton * tmpButt = dynamic_cast<RToolButton *>(tmpObj);
                if(tmpButt && tmpButt->objectName() == QString(ID))
                {
                    toolButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
                    d->toolBarLayout->insertWidget(index,toolButton);
                    return true;
                }
                index++;
            }
        }
        return false;
    }
    return true;
}
