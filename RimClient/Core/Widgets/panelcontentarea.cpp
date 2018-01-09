#include "panelcontentarea.h"

#include <QHBoxLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QResizeEvent>
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
#include "rsingleton.h"

#define PANEL_ITEM_WIDTH 24

class PanelContentAreaPrivate: public GlobalData<PanelContentArea>
{
    Q_DECLARE_PUBLIC(PanelContentArea)

private:
    PanelContentAreaPrivate(PanelContentArea * q):
        q_ptr(q)
    {

    }
    PanelContentArea * q_ptr;


    QWidget * mainWidget;

    PanelTabBar * panelTabBar;
    QStackedWidget * panelStackedWidget;

    PanelPersonPage * personWidget;
    PanelHistoryPage * historyWidget;
    PanelGroupPage * groupWidget;
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
    iconLabel = new QLabel(contentWidget);
    iconLabel->setFixedSize(PANEL_ITEM_WIDTH,PANEL_ITEM_WIDTH);

    textLabel = new QLabel(contentWidget);
    textLabel->setMinimumHeight(PANEL_ITEM_WIDTH);
    textLabel->setVisible(false);

    contentLayout->addStretch(1);
    contentLayout->addWidget(iconLabel);
    contentLayout->addSpacing(3);
    contentLayout->addWidget(textLabel);
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

    iconLabel->setStyleSheet(QString("border-image:url(%1)").arg(QString(":/icon/resource/icon/%1_%2.png").arg(objectName()).arg(flag)));

    contentWidget->setProperty("selected",flag);
    contentWidget->style()->unpolish(contentWidget);
    contentWidget->style()->polish(contentWidget);
}

void PanelTabItem::setTextVisible(bool flag)
{
    textLabel->setVisible(flag);
}

void PanelTabItem::setText(const QString &text)
{
    textLabel->setText(text);
}

QString PanelTabItem::text() const
{
    return textLabel->text();
}

void PanelTabItem::mousePressEvent(QMouseEvent *)
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
    d_ptr(new PanelContentAreaPrivate(this)),
    QWidget(parent)
{
    RSingleton<Subject>::instance()->attach(this);
    initWidget();
}

PanelContentArea::~PanelContentArea()
{
    RSingleton<Subject>::instance()->detach(this);
    delete d_ptr;
}

void PanelContentArea::onMessage(MessageType)
{

}

void PanelContentArea::initWidget()
{
    MQ_D(PanelContentArea);
    QHBoxLayout * layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setMargin(0);

    d->mainWidget = new QWidget(this);
    d->mainWidget->setObjectName("MainWidget");
    layout->addWidget(d->mainWidget);
    this->setLayout(layout);

    QVBoxLayout * mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    d->panelTabBar = new PanelTabBar(d->mainWidget);
    d->panelStackedWidget = new QStackedWidget(d->mainWidget);

    PanelTabItem * tabItem = d->panelTabBar->addTab();
    tabItem->setObjectName("person");
    tabItem->setToolTip(tr("Person"));

    tabItem = d->panelTabBar->addTab();
    tabItem->setObjectName("group");
    tabItem->setToolTip(tr("Group"));

    tabItem = d->panelTabBar->addTab();
    tabItem->setObjectName("history");
    tabItem->setToolTip(tr("History"));

    d->personWidget = new PanelPersonPage(this);
    d->groupWidget = new PanelGroupPage(this);
    d->historyWidget = new PanelHistoryPage(this);

    d->panelStackedWidget->addWidget(d->personWidget);
    d->panelStackedWidget->addWidget(d->groupWidget);
    d->panelStackedWidget->addWidget(d->historyWidget);

    connect(d->panelTabBar,SIGNAL(currentIndexChanged(int)),d->panelStackedWidget,SLOT(setCurrentIndex(int)));

    d->panelTabBar->setCurrentIndex(0);

    mainLayout->addWidget(d->panelTabBar);
    mainLayout->addWidget(d->panelStackedWidget);
    d->mainWidget->setLayout(mainLayout);
}
