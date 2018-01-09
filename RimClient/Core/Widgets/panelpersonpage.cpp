#include "panelpersonpage.h"

#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "maindialog.h"
#include "actionmanager/actionmanager.h"

#include "toolbox/toolbox.h"

class PanelPersonPagePrivate : public GlobalData<PanelPersonPage>
{
    Q_DECLARE_PUBLIC(PanelPersonPage)
protected:
    PanelPersonPagePrivate(PanelPersonPage * q):
        q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    PanelPersonPage * q_ptr;
    ToolBox * toolBox;

    QWidget * contentWidget;

    QList<ToolPage *> pages;
    QList<ToolItem *> toolItems;
};

void PanelPersonPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    toolBox = new ToolBox(contentWidget);
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(toolBox);
    contentWidget->setLayout(contentLayout);
}

PanelPersonPage::PanelPersonPage(QWidget *parent):
    d_ptr(new PanelPersonPagePrivate(this)),
    QWidget(parent)
{
    createAction();

    for(int j = 0; j < 2;j++)
    {
        ToolPage * page = d_ptr->toolBox->addPage(QStringLiteral("我的好友"));
        d_ptr->pages.append(page);
        for(int i = 0; i < 5;i++)
        {
            ToolItem * item = new ToolItem(page);
            connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
            connect(item,SIGNAL(showChatWindow(ToolItem*)),this,SLOT(createChatWindow(ToolItem*)));
            connect(item,SIGNAL(itemDoubleClick(ToolItem*)),MainDialog::instance(),SLOT(showChatWindow(ToolItem*)));
            connect(item,SIGNAL(itemMouseHover(bool,ToolItem*)),MainDialog::instance(),SLOT(showHoverItem(bool,ToolItem*)));
            item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM));
            item->setName(QString(QStringLiteral("韩天才%1")).arg(i));
            item->setNickName("MagnyCopper");
            item->setDescInfo(QStringLiteral("PSN确实快了很多啊！！！^_^"));
            page->addItem(item);
            d_ptr->toolItems.append(item);
        }

        page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
    }
}

PanelPersonPage::~PanelPersonPage()
{

}

/*!
     * @brief 刷新当前用户列表，重新发送请求至服务器
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelPersonPage::refreshList()
{

}

/*!
     * @brief 添加新分组
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelPersonPage::addGroup()
{

}

void PanelPersonPage::renameGroup()
{
    MQ_D(PanelPersonPage);
    ToolPage * page = d->toolBox->selectedPage();
    if(page)
    {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<page;
    }
}

void PanelPersonPage::delGroup()
{

}

void PanelPersonPage::createChatWindow(ToolItem *item)
{
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"__"<<item;
}

void PanelPersonPage::sendInstantMessage()
{
     MQ_D(PanelPersonPage);
    createChatWindow(d->toolBox->selectedItem());
}

void PanelPersonPage::showUserDetail()
{

}

void PanelPersonPage::modifyUserInfo()
{

}

void PanelPersonPage::deleteUser()
{

}

void PanelPersonPage::createAction()
{
    MQ_D(PanelPersonPage);

    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLBOX);

    QAction * freshAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_REFRESH,this,SLOT(refreshList()));
    freshAction->setText(tr("Refresh list"));

    QAction * addGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_ADDGROUP,this,SLOT(addGroup()));
    addGroupAction->setText(tr("Add group"));

    menu->addAction(freshAction);
    menu->addSeparator();
    menu->addAction(addGroupAction);

    d->toolBox->setContextMenu(menu);

    //创建分组的右键菜单
    QMenu * groupMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLGROUP);

    QAction * renameAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_RENAME,this,SLOT(renameGroup()));
    renameAction->setText(tr("Rename"));

    QAction * deleteGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_DELGROUP,this,SLOT(delGroup()));
    deleteGroupAction->setText(tr("Delete group"));

    groupMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_REFRESH));
    groupMenu->addSeparator();
    groupMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_ADDGROUP));
    groupMenu->addAction(renameAction);
    groupMenu->addAction(deleteGroupAction);

    //创建联系人的右键菜单
    QMenu * personMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLITEM);

    QAction * sendMessAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_SENDMESSAGE,this,SLOT(sendInstantMessage()));
    sendMessAction->setText(tr("Send Instant Message"));

    QAction * viewDetailAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_VIEWDETAIL,this,SLOT(showUserDetail()));
    viewDetailAction->setText(tr("View Detail"));

    QAction * modifyAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_MODIFYCOMMENTS,this,SLOT(modifyUserInfo()));
    modifyAction->setText(tr("Modify Comments"));

    QAction * deletePersonAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_DELPERSON,this,SLOT(deleteUser()));
    deletePersonAction->setText(tr("Delete Friend"));

    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_SENDMESSAGE));
    personMenu->addSeparator();
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_VIEWDETAIL));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MODIFYCOMMENTS));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_DELPERSON));
    personMenu->addSeparator();
}

