#include "panelgrouppage.h"

#include <QLineEdit>
#include <QHBoxLayout>

#include "head.h"
#include "global.h"
#include "datastruct.h"
#include "constants.h"
#include "rsingleton.h"
#include "Network/msgwrap.h"
#include "actionmanager/actionmanager.h"

#include "toolbox/toolbox.h"
using namespace ProtocolType;

class PanelGroupPagePrivate : public GlobalData<PanelGroupPage>
{
    Q_DECLARE_PUBLIC(PanelGroupPage)

public:
    PanelGroupPagePrivate(PanelGroupPage * q):
        q_ptr(q)
    {
        groupIsCreate = false;
        initWidget();
    }

    void initWidget();

    ToolBox * toolBox;
    QList<ToolPage *> pages;
    QList<ToolItem *> toolItems;
    QLineEdit *tmpNameEdit;     //重命名时用edit
    ToolPage * pageOfMovedItem; //待移动群分组的item所属的page
    bool groupIsCreate;         //标识分组是新创建还是已存在
    PanelGroupPage * q_ptr;

    QWidget * contentWidget;
};

void PanelGroupPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    toolBox = new ToolBox(contentWidget);
    QObject::connect(toolBox,SIGNAL(updateGroupActions(ToolPage *)),q_ptr,SLOT(updateGroupActions(ToolPage *)));
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(toolBox);
    contentWidget->setLayout(contentLayout);

    tmpNameEdit = new QLineEdit(toolBox);
    QObject::connect(tmpNameEdit,SIGNAL(editingFinished()),q_ptr,SLOT(renameEditFinished()));
    tmpNameEdit->setPlaceholderText(QObject::tr("untitled"));
    tmpNameEdit->hide();

}

PanelGroupPage::PanelGroupPage(QWidget *parent) : QWidget(parent),
    d_ptr(new PanelGroupPagePrivate(this))
{
    createAction();

    for(int j = 0; j < 5;j++)
    {
        ToolPage * page = d_ptr->toolBox->addPage(QStringLiteral("我的群")+QString::number(j));
        d_ptr->pages.append(page);
        for(int i = 0; i < 5;i++)
        {
            ToolItem * item = new ToolItem(page);
            connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
            connect(item,SIGNAL(updateGroupActions()),page,SLOT(updateGroupActions()));
            item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLITEM));
            item->setName(QStringLiteral("天地仁谷"));
            item->setNickName(QStringLiteral("(16人)"));
            page->addItem(item);
            d_ptr->toolItems.append(item);
        }

        page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLGROUP));
    }
}

PanelGroupPage::~PanelGroupPage()
{

}

void PanelGroupPage::searchGroup()
{

}

/*!
     * @brief 创建一个群
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelGroupPage::newGroup()
{

}

/*!
     * @brief 添加群分组
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelGroupPage::addGroups()
{
    MQ_D(PanelGroupPage);
    d->groupIsCreate = true;
    ToolPage * page = d->toolBox->addPage(QStringLiteral("untitled"));
    page->setDefault(false);
    d->pages.append(page);
    page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLGROUP));
    QRect textRec = d->toolBox->penultimatePage()->textRect();
    QRect pageRec = d->toolBox->penultimatePage()->geometry();
    int textY = pageRec.y()+page->txtFixedHeight();
    if(d->toolBox->penultimatePage()->isExpanded())
    {
        textY = pageRec.y()+pageRec.height();
    }

    d->tmpNameEdit->raise();
    d->tmpNameEdit->setText(page->toolName());
    d->tmpNameEdit->selectAll();
    d->tmpNameEdit->setGeometry(textRec.x(),textY,pageRec.width(),page->txtFixedHeight());
    d->tmpNameEdit->show();
    d->tmpNameEdit->setFocus();
}

/*!
     * @brief 重命名分组
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelGroupPage::renameGroup()
{
    MQ_D(PanelGroupPage);
    ToolPage * page = d->toolBox->selectedPage();
    if(page)
    {
        d->groupIsCreate = false;

        QRect textRec = page->textRect();
        QRect pageRec = page->geometry();
        d->tmpNameEdit->raise();
        d->tmpNameEdit->setText(tr("untitled"));
        d->tmpNameEdit->selectAll();
        d->tmpNameEdit->setGeometry(textRec.x(),textRec.y(),pageRec.width(),textRec.height());
        d->tmpNameEdit->show();
        d->tmpNameEdit->setFocus();
    }
}

void PanelGroupPage::deleteGroup()
{

}

void PanelGroupPage::sendMessage()
{

}

void PanelGroupPage::viewDetail()
{

}

void PanelGroupPage::modifyGroupInfo()
{

}

void PanelGroupPage::exitGroup()
{

}

/*!
     * @brief LineEdit中命名完成后将内容设置为page的name
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelGroupPage::renameEditFinished()
{
    MQ_D(PanelGroupPage);
    if(d->tmpNameEdit->text() != NULL)
    {
        d->toolBox->selectedPage()->setToolName(d->tmpNameEdit->text());
        GroupingRequest * request = new GroupingRequest();
        request->uuid = G_UserBaseInfo.uuid;
        if(d->groupIsCreate)
        {
            request->type = GROUPING_CREATE;
        }
        else
        {
            request->type = GROUPING_RENAME;
        }
        request->gtype = GROUPING_GROUP;
        request->groupName = d->tmpNameEdit->text();
        request->groupId = d->toolBox->selectedPage()->getID();
        RSingleton<MsgWrap>::instance()->handleMsg(request);

        d->groupIsCreate = true;
    }
    d->tmpNameEdit->setText("");
    d->tmpNameEdit->hide();
}

/*!
     * @brief 根据触发右键菜单的Item所属的Page来添加“移动群至”菜单中Action
     * @param[in] page：ToolPage *
     * @return 无
     *
     */
void PanelGroupPage::updateGroupActions(ToolPage * page)
{
    MQ_D(PanelGroupPage);
    d->pageOfMovedItem = page;
    QList <PersonGroupInfo> infoList = d->toolBox->toolPagesinfos();
    QMenu * moveGroupTo  = ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLITEM_GROUPS);
    if(!moveGroupTo->isEmpty())
    {
        moveGroupTo->clear();
    }
    for(int index=0;index<infoList.count();index++)
    {
        if(page->pageInfo().uuid != infoList.at(index).uuid)
        {
            QAction *tmpAction = new QAction(infoList.at(index).name);
            tmpAction->setData(infoList.at(index).uuid);
            connect(tmpAction,SIGNAL(triggered(bool)),this,SLOT(moveGroupTo()));
            moveGroupTo->addAction(tmpAction);
        }
        else
        {
            continue;
        }
    }
}

/*!
     * @brief 移动群组至各分组的Action响应
     * @details 只处理移动的请求，待服务器移动成功后，再真实的移动
     * @param
     * @return 无
     */
void PanelGroupPage::moveGroupTo()
{
    MQ_D(PanelGroupPage);
    QAction * target = qobject_cast<QAction *>(QObject::sender());
    QString targetUuid = target->data().toString();
    ToolPage * targetPage = d->toolBox->targetPage(targetUuid);
    ToolPage * sourcePage = d->pageOfMovedItem;
    ToolItem * targetItem = d->toolBox->selectedItem();
    if(!targetPage||!sourcePage)
    {
        return;
    }
    else
    {
        GroupingFriendRequest * request = new GroupingFriendRequest;
//        request->type = G_Friend_MOVE;
//        request->stype = SearchPerson;
//        request->groupId = targetPage->getID();
//        request->oldGroupId = sourcePage->getID();

//        UserClient * client = RSingleton<UserManager>::instance()->client(targetItem);
//        if(client)
//        {
//            request->user = client->simpleUserInfo;
//        }
        RSingleton<MsgWrap>::instance()->handleMsg(request);
    }
}


void PanelGroupPage::createAction()
{
    MQ_D(PanelGroupPage);

    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLBOX);

    QAction * searchGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_SEARCHGROUP,this,SLOT(searchGroup()));
    searchGroupAction->setText(tr("Search group"));

    QAction * newGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_ADDGROUP,this,SLOT(newGroup()));
    newGroupAction->setText(tr("New group"));

    QAction * addGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_ADDGROUPS,this,SLOT(addGroups()));
    addGroupAction->setText(tr("Add groups"));

    menu->addAction(searchGroupAction);
    menu->addAction(newGroupAction);
    menu->addSeparator();
    menu->addAction(addGroupAction);

    d->toolBox->setContextMenu(menu);

    //创建分组的右键菜单
    QMenu * groupMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLGROUP);

    QAction * renameAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_RENAME,this,SLOT(renameGroup()));
    renameAction->setText(tr("Rename group"));

    QAction * delGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_DELGROUP,this,SLOT(deleteGroup()));
    delGroupAction->setText(tr("Delete group"));

    groupMenu->addAction(searchGroupAction);
    groupMenu->addAction(newGroupAction);
    groupMenu->addSeparator();
    groupMenu->addAction(addGroupAction);
    groupMenu->addAction(renameAction);
    groupMenu->addAction(delGroupAction);


    //创建item的右键菜单
    QMenu * itemMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLITEM);

    QAction * sendMessAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_SENDMESS,this,SLOT(sendMessage()));
    sendMessAction->setText(tr("Send message"));

    QAction * viewDetailAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_VIEWDETAIL,this,SLOT(viewDetail()));
    viewDetailAction->setText(tr("View detail"));

    QAction * modifyAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_MODIFYCOMMENTS,this,SLOT(modifyGroupInfo()));
    modifyAction->setText(tr("Modify name"));

    QAction * exitGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_EXITGROUP,this,SLOT(exitGroup()));
    exitGroupAction->setText(tr("Exit group"));

    QAction * moveGroupToAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_MOVEGROUPTO);
    moveGroupToAction->setText(tr("Move Group To"));

    QMenu * groupsMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLITEM_GROUPS);
    moveGroupToAction->setMenu(groupsMenu);

    itemMenu->addAction(sendMessAction);
    itemMenu->addSeparator();
    itemMenu->addAction(viewDetailAction);
    itemMenu->addSeparator();
    itemMenu->addAction(modifyAction);
    itemMenu->addAction(moveGroupToAction);
    itemMenu->addSeparator();
    itemMenu->addAction(exitGroupAction);
}
