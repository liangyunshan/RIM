#include "panelbottomtoolbar.h"

#include <QHBoxLayout>
#include <QToolButton>

#include "head.h"
#include "constants.h"

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

    toolButton = new QToolButton();
    toolButton->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    toolButton->setObjectName("Panel_Bottom_ToolButton");
    toolButton->setToolTip(tr("Main menu"));
    toolButton->setIcon(QIcon(Constant::Panel_Bottom_ToolButton));

    searchPerson = new QToolButton();
    searchPerson->setFixedSize(PANEL_BOTTOM_TOOL_WIDTH,PANEL_BOTTOM_TOOL_HEIGHT);
    searchPerson->setObjectName("Panel_Bottom_AddPersonButton");
    searchPerson->setToolTip(tr("Add Person"));
    searchPerson->setIcon(QIcon(Constant::Panel_Bottom_AddPersonButton));

    contentLayout->addWidget(toolButton);
    contentLayout->addWidget(searchPerson);
    contentLayout->addStretch(1);

    mainWidget->setLayout(contentLayout);
}
