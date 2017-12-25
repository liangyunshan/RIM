#include "panelbottomtoolbar.h"

#include <QHBoxLayout>
#include <QToolButton>

#include "head.h"
#include "constants.h"
#include "actionmanager/actionmanager.h"
#include "addfriend.h"
#include "maindialog.h"

#define PANEL_BOTTOM_TOOL_WIDTH 20
#define PANEL_BOTTOM_TOOL_HEIGHT 40

PanelBottomToolBar::PanelBottomToolBar(QWidget *parent):
    QWidget(parent)
{
   initWidget();
}

PanelBottomToolBar::~PanelBottomToolBar()
{

}

void PanelBottomToolBar::initWidget()
{
    mainWidget = new QWidget(this);
    mainWidget->setObjectName("Panel_Bottom_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(mainWidget);
    this->setLayout(mainLayout);

    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(6,1,1,1);
    contentLayout->setSpacing(2);

    RToolButton * toolButton = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_TOOL,NULL,NULL);
    toolButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    toolButton->setToolTip(tr("Main menu"));

    RToolButton * searchPerson = ActionManager::instance()->createToolButton(Constant::TOOL_PANEL_ADDPERSON,this,SLOT(showAddFriendPanel()));
    searchPerson->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    searchPerson->setToolTip(tr("Add Person"));

    contentLayout->addWidget(toolButton);
    contentLayout->addWidget(searchPerson);
    contentLayout->addStretch(1);

    mainWidget->setLayout(contentLayout);
}

void PanelBottomToolBar::showAddFriendPanel()
{
    AddFriend * adf = new AddFriend();
    adf->show();
}
