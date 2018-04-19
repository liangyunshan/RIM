#include "panelhistorypage.h"

#include <QHBoxLayout>
#include <QMenu>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbox/listbox.h"

#include "actionmanager/actionmanager.h"

class PanelHistoryPagePrivate : public GlobalData<PanelHistoryPage>
{
    Q_DECLARE_PUBLIC(PanelHistoryPage)

public:
    PanelHistoryPagePrivate(PanelHistoryPage * q):
        q_ptr(q)
    {
        initWidget();
    }
    void initWidget();

    PanelHistoryPage * q_ptr;
    ListBox * listBox;
     QWidget * contentWidget;
};

void PanelHistoryPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    listBox = new ListBox(contentWidget);
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(listBox);
    contentWidget->setLayout(contentLayout);
}

PanelHistoryPage::PanelHistoryPage(QWidget *parent):
    d_ptr(new PanelHistoryPagePrivate(this)),
    QWidget(parent)
{
    createAction();
    for(int i = 0; i < 5;i++)
    {
        ToolItem * item = new ToolItem(NULL);
        item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_HISTORY_TOOLITEM));
        item->setName(QString::number(i));
        item->setNickName("");
        item->setDescInfo("");
        d_ptr->listBox->addItem(item);
    }
    d_ptr->listBox->setContextMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_HISTORY_TOOLBOX));
}

PanelHistoryPage::~PanelHistoryPage()
{

}

void PanelHistoryPage::clearList()
{

}

void PanelHistoryPage::sendInstantMessage()
{

}

/*!
 * @brief 将历史会话记录从会话列表中移除
 */
void PanelHistoryPage::removeSessionFromList()
{
    MQ_D(PanelHistoryPage);
    d->listBox->removeItem(d->listBox->selectedItem());
}

void PanelHistoryPage::createAction()
{
    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_HISTORY_TOOLBOX);

    QAction * clearListAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_CLEARLIST,this,SLOT(clearList()));
    clearListAction->setText(tr("Clear list"));

    menu->addAction(clearListAction);

    //创建联系人的右键菜单
    QMenu * historyMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_HISTORY_TOOLITEM);

    QAction * stickSessionAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_STICKSESSION,this,SLOT(sendInstantMessage()));
    stickSessionAction->setText(tr("Top session"));

    QAction * removeSessionAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_REMOVEFORMLIST,this,SLOT(removeSessionFromList()));
    removeSessionAction->setText(tr("Remove session"));

    historyMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_SENDMESSAGE));
    historyMenu->addSeparator();
    historyMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_VIEWDETAIL));
    historyMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_DELPERSON));
    historyMenu->addSeparator();
    historyMenu->addSeparator();
    historyMenu->addAction(stickSessionAction);
    historyMenu->addAction(removeSessionAction);
}
