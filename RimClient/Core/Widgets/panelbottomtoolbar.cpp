#include "panelbottomtoolbar.h"

#include <QHBoxLayout>
#include <QToolButton>

#include "head.h"
#include "constants.h"
#include "actionmanager/actionmanager.h"
#include "addfriend.h"
#include "maindialog.h"
#include "rsingleton.h"
#include "systemsettings.h"

#define PANEL_BOTTOM_TOOL_WIDTH 20
#define PANEL_BOTTOM_TOOL_HEIGHT 40

class PanelBottomToolBarPrivate : public GlobalData<PanelBottomToolBar>
{
    Q_DECLARE_PUBLIC(PanelBottomToolBar)

private:
    PanelBottomToolBarPrivate(PanelBottomToolBar * q):q_ptr(q)
    {
        addFriendInstance = NULL;
        settingsInstance = NULL;
        initWidget();
    }

    ~PanelBottomToolBarPrivate()
    {
        if(addFriendInstance)
        {
            addFriendInstance->close();
        }

        if(settingsInstance)
        {
            settingsInstance->close();
        }
    }

    void initWidget();

    PanelBottomToolBar * q_ptr;

    AddFriend * addFriendInstance;
    SystemSettings * settingsInstance;

    QWidget * mainWidget;
};

void PanelBottomToolBarPrivate::initWidget()
{
    mainWidget = new QWidget(q_ptr);
    mainWidget->setObjectName("Panel_Bottom_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(mainWidget);
    q_ptr->setLayout(mainLayout);

    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(6,1,1,1);
    contentLayout->setSpacing(2);

    RToolButton * toolButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_TOOL,q_ptr,SLOT(showSystemSetting()));
    toolButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    toolButton->setToolTip(QObject::tr("Main menu"));

    RToolButton * searchPerson = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_ADDPERSON,q_ptr,SLOT(showAddFriendPanel()));
    searchPerson->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    searchPerson->setToolTip(QObject::tr("Add Person"));

    contentLayout->addWidget(toolButton);
    contentLayout->addWidget(searchPerson);
    contentLayout->addStretch(1);

    mainWidget->setLayout(contentLayout);
}

PanelBottomToolBar::PanelBottomToolBar(QWidget *parent):
    d_ptr(new PanelBottomToolBarPrivate(this)),
    QWidget(parent)
{
    if(!d_ptr->settingsInstance)
    {
        d_ptr->settingsInstance = new SystemSettings();
        connect(d_ptr->settingsInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateSettingInstnce(QObject*)));
    }

    RSingleton<Subject>::instance()->attach(this);
}

PanelBottomToolBar::~PanelBottomToolBar()
{
    RSingleton<Subject>::instance()->detach(this);
    delete d_ptr;
}

void PanelBottomToolBar::onMessage(MessageType)
{

}

void PanelBottomToolBar::showAddFriendPanel()
{
    MQ_D(PanelBottomToolBar);
    if(!d->addFriendInstance)
    {
        d->addFriendInstance = new AddFriend();
        connect(d->addFriendInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateFrinedInstance(QObject*)));
    }

    d->addFriendInstance->showNormal();
}

void PanelBottomToolBar::showSystemSetting()
{
    MQ_D(PanelBottomToolBar);
    if(!d->settingsInstance)
    {
        d->settingsInstance = new SystemSettings();
        connect(d->settingsInstance,SIGNAL(destroyed(QObject*)),this,SLOT(updateSettingInstnce(QObject*)));
    }

    d->settingsInstance->showNormal();
}

void PanelBottomToolBar::updateFrinedInstance(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->addFriendInstance)
    {
        d->addFriendInstance = NULL;
    }
}

void PanelBottomToolBar::updateSettingInstnce(QObject *)
{
    MQ_D(PanelBottomToolBar);
    if(d->settingsInstance)
    {
        d->settingsInstance = NULL;
    }
}
