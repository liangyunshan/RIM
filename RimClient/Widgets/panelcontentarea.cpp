#include "panelcontentarea.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QResizeEvent>
#include <QDebug>
#include <QAction>
#include <QPushButton>
#include <QToolButton>
#include <QStyle>
#include <QLabel>

#include "datastruct.h"
#include "constants.h"
#include "head.h"
#include "panelpersonpage.h"
#include "panelgrouppage.h"
#include "panelhistorypage.h"

#define PANEL_ITEM_WIDTH 24

class PanelContentAreaPrivate: public GlobalData<PanelContentArea>
{
    Q_DECLARE_PUBLIC(PanelContentArea)

};

PanelTabItem::PanelTabItem(QWidget *parent):QWidget(parent)
{
    selected = false;

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);

    contentWidget = new QWidget(this);
    contentWidget->setObjectName("PanelTabItemWidget");
    layout->addWidget(contentWidget);
    this->setLayout(layout);

    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    label = new QLabel(contentWidget);
    label->setFixedSize(PANEL_ITEM_WIDTH,PANEL_ITEM_WIDTH);

    contentLayout->addStretch(1);
    contentLayout->addWidget(label);
    contentLayout->addStretch(1);
    contentWidget->setLayout(contentLayout);
}

bool PanelTabItem::isSelected()
{
    return this->selected;
}

void PanelTabItem::setSelected(bool flag)
{
    selected = flag;

    label->setStyleSheet(QString("border-image:url(%1)").arg(QString(":/icon/resource/icon/%1_%2.png").arg(objectName()).arg(flag)));

    contentWidget->setProperty("selected",flag);
    contentWidget->style()->unpolish(contentWidget);
    contentWidget->style()->polish(contentWidget);
}

void PanelTabItem::mousePressEvent(QMouseEvent *event)
{
    emit selectedMe();
}


PanelTabBar::PanelTabBar(QWidget *parent):
    QWidget(parent)
{
    setMinimumHeight(38);

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    contentWidget = new QWidget(this);
    mainLayout->addWidget(contentWidget);

    contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentWidget->setLayout(contentLayout);

    itemSelectedIndex = -1;
}

PanelTabItem * PanelTabBar::addTab()
{
    PanelTabItem * item = new PanelTabItem();

    connect(item,SIGNAL(selectedMe()),this,SLOT(switchItemIndex()));

    contentLayout->addWidget(item);
    tabItems.append(item);

    return item;
}

void PanelTabBar::removeTab(int index)
{
    if(index >= 0 && index < tabItems.size())
    {
        contentLayout->removeWidget(tabItems.at(index));
        //Yang 20171215待完善对itemSelectedIndex修改
    }
}

void PanelTabBar::setCurrentIndex(int index)
{
    if(index >= 0 && index < tabItems.size())
    {
        itemSelectedIndex = index;
        for(int i = 0; i < tabItems.size(); i++)
        {
            if(i == index)
            {
                tabItems.at(i)->setSelected(true);
                emit currentIndexChanged(i);
            }
            else
            {
                tabItems.at(i)->setSelected(false);
            }
        }
    }
}

int PanelTabBar::currentIndex()
{
    return itemSelectedIndex;
}

void PanelTabBar::resizeEvent(QResizeEvent *)
{

}

void PanelTabBar::switchItemIndex()
{
    PanelTabItem * tabBar = dynamic_cast<PanelTabItem *>(QObject::sender());
    if(tabBar)
    {
        for(int i = 0; i < tabItems.size(); i++)
        {
            if(tabItems.at(i) == tabBar)
            {
                setCurrentIndex(i);
                break;
            }
        }
    }
}

PanelContentArea::PanelContentArea(QWidget *parent):
    d_ptr(new PanelContentAreaPrivate()),
    QWidget(parent)
{
    initWidget();
}

PanelContentArea::~PanelContentArea()
{

}

void PanelContentArea::initWidget()
{
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setMargin(0);

    mainWidget = new QWidget(this);
    mainWidget->setObjectName("MainWidget");
    layout->addWidget(mainWidget);
    this->setLayout(layout);


    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    panelTabBar = new PanelTabBar(mainWidget);
    panelStackedWidget = new QStackedWidget(mainWidget);

    panelTabBar->addTab()->setObjectName("person");
    panelTabBar->addTab()->setObjectName("group");
    panelTabBar->addTab()->setObjectName("history");

    personWidget = new PanelPersonPage(this);
    groupWidget = new PanelGroupPage(this);
    historyWidget = new PanelHistoryPage(this);

    panelStackedWidget->addWidget(personWidget);
    panelStackedWidget->addWidget(groupWidget);
    panelStackedWidget->addWidget(historyWidget);

    connect(panelTabBar,SIGNAL(currentIndexChanged(int)),panelStackedWidget,SLOT(setCurrentIndex(int)));

    panelTabBar->setCurrentIndex(0);

    mainLayout->addWidget(panelTabBar);
    mainLayout->addWidget(panelStackedWidget);
    mainWidget->setLayout(mainLayout);
}
