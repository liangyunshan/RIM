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
#include "user/userclient.h"
#include "messdiapatch.h"
#include "user/user.h"
#include "user/groupclient.h"
#include "user/userchatcontainer.h"
#include "registgroupdialog.h"

#include "toolbox/toolbox.h"
using namespace ProtocolType;

class PanelGroupPagePrivate : public GlobalData<PanelGroupPage>
{
    Q_DECLARE_PUBLIC(PanelGroupPage)

public:
    PanelGroupPagePrivate(PanelGroupPage * q):
        q_ptr(q),isGroupHasCreated(false)
    {
        groupIsCreate = false;
        initWidget();
    }

    void initWidget();

    PanelGroupPage * q_ptr;
    ToolBox * toolBox;

    QList<ToolItem *> toolItems;
    QLineEdit *tmpNameEdit;                 //重命名时用edit
    ToolPage * pageOfMovedItem;             //待移动群分组的item所属的page
    ToolItem * m_movedItem;                 //待移动的联系人item
    bool groupIsCreate;                     //标识分组是新创建还是已存在
    QString m_deleteID;                     //暂时将删除的分组ID保存在内存中
    bool isGroupHasCreated;                 //标识分组列表是第一次创建还是刷新显示
    QString pageNameBeforeRename;           //重命名前分组名称，为了避免在步修改名称下依然更新服务器

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
    QObject::connect(toolBox,SIGNAL(pageIndexMoved(int,QString)),q_ptr,SLOT(respGroupMoved(int,QString)));
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(toolBox);
    contentWidget->setLayout(contentLayout);

    tmpNameEdit = new QLineEdit(toolBox);
    QObject::connect(tmpNameEdit,SIGNAL(editingFinished()),q_ptr,SLOT(renameEditFinished()));
    tmpNameEdit->setPlaceholderText(QObject::tr("untitled"));
    tmpNameEdit->hide();

    pageOfMovedItem = NULL;
    m_movedItem = NULL;
}

PanelGroupPage::PanelGroupPage(QWidget *parent) : QWidget(parent),
    d_ptr(new PanelGroupPagePrivate(this))
{
    createAction();

    connect(MessDiapatch::instance(),SIGNAL(recvGroupList(MsgOperateResponse,ChatGroupListResponse*)),
                            this,SLOT(updateGroupList(MsgOperateResponse,ChatGroupListResponse*)));
    connect(MessDiapatch::instance(),SIGNAL(recvGroupGroupingOperate(GroupingResponse)),this,SLOT(recvGroupGroupingOperate(GroupingResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvRegistGroup(RegistGroupResponse)),this,SLOT(recvRegistGroup(RegistGroupResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvRegistGroupFailed()),this,SLOT(recvRegistGroupFailed()));
}

void PanelGroupPage::onMessage(MessageType type)
{
    MQ_D(PanelGroupPage);
    switch(type)
    {
        default:
            break;
    }
}

void PanelGroupPage::updateGroupList(MsgOperateResponse status, ChatGroupListResponse *response)
{
    MQ_D(PanelGroupPage);
    RSingleton<UserChatContainer>::instance()->reset(response->groups);

    if(!d->isGroupHasCreated){
        createGroup();
    }else{
//        updateContactList();
    }
}

void PanelGroupPage::searchGroup()
{

}

/*!
 * @brief 创建新群
 */
void PanelGroupPage::createNewGroup()
{
    RegistGroupDialog * dialog = new RegistGroupDialog();
    connect(this,SIGNAL(registChatGroupResult(bool)),dialog,SLOT(recvRegistResult(bool)));
    dialog->show();
}

/*!
 * @brief 添加群分组
 * @param[in] 无
 * @return 无
 */
void PanelGroupPage::respGroupCreate()
{
    MQ_D(PanelGroupPage);

    d->groupIsCreate = true;

    ToolPage * page = d->toolBox->addPage(QStringLiteral("untitled"));
    page->setDefault(false);
    page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLGROUP));

    QRect textRec = d->toolBox->penultimatePage()->textRect();
    QRect pageRec = d->toolBox->penultimatePage()->geometry();

    int textY = pageRec.y()+page->txtFixedHeight();
    if(d->toolBox->penultimatePage()->isExpanded()){
        textY = pageRec.y() + pageRec.height();
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
 * @details 待光标离开输入区域时，自动将输入的信息发送更新至服务器
 * @return 无
 */
void PanelGroupPage::respGroupRename()
{
    MQ_D(PanelGroupPage);
    ToolPage * page = d->toolBox->selectedPage();
    if(page){
        d->groupIsCreate = false;

        QRect textRec = page->textRect();
        QRect pageRec = page->geometry();
        d->pageNameBeforeRename = page->toolName();
        d->tmpNameEdit->raise();
        d->tmpNameEdit->setText(page->toolName());
        d->tmpNameEdit->selectAll();
        d->tmpNameEdit->setGeometry(textRec.x(),pageRec.y(),pageRec.width(),textRec.height());
        d->tmpNameEdit->show();
        d->tmpNameEdit->setFocus();
    }
}

/*!
 * @brief 删除群分组
 * @param[in] 无
 * @return 无
 */
void PanelGroupPage::respGroupDeleted()
{
    MQ_D(PanelGroupPage);

    ToolPage *t_page = d->toolBox->selectedPage();
    if(t_page->isDefault())
    {
        return;
    }
    d->m_deleteID = t_page->getID();

    GroupingRequest * request = new GroupingRequest();
    request->uuid = G_User->BaseInfo().uuid;
    request->type = GROUPING_DELETE;
    request->gtype = OperateGroup;
    request->groupId = t_page->getID();

    RSingleton<MsgWrap>::instance()->handleMsg(request);
}

/*!
 * @brief 响应分组移动
 * @param[in] index 重新排序后，分组在列表中索引
 * @param[in] pageId 执行排序的分组ID
 */
void PanelGroupPage::respGroupMoved(int index, QString pageId)
{
    MQ_D(PanelGroupPage);

    GroupingRequest * request = new GroupingRequest();
    request->uuid = G_User->BaseInfo().uuid;
    request->type = GROUPING_SORT;
    request->gtype = OperateGroup;
    request->groupName = d->tmpNameEdit->text();
    request->groupId = pageId;
    request->groupIndex = index;

    RSingleton<MsgWrap>::instance()->handleMsg(request);
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
 * @brief 处理分组操作结果
 * @param[in] response 操作结果响应
 */
void PanelGroupPage::recvGroupGroupingOperate(GroupingResponse response)
{
    MQ_D(PanelGroupPage);
    if(response.uuid != G_User->BaseInfo().uuid)
        return;

    switch(response.type){
        case GROUPING_CREATE:{
                RSingleton<UserChatContainer>::instance()->addGroup(response.groupId,response.groupIndex);
            }
            break;
        case GROUPING_RENAME:{
                RSingleton<UserChatContainer>::instance()->renameGroup(response.groupId);
            }
            break;
        case GROUPING_DELETE:{
                clearTargetGroup(d->m_deleteID);
            }
            break;
        case GROUPING_SORT:{
                d->toolBox->sortPage(response.groupId,response.groupIndex);
                RSingleton<UserChatContainer>::instance()->sortGroup(response.groupId,response.groupIndex);
            }
            break;
        default:break;
    }
    updateGroupDescInfo();
}

/*!
 * @brief 处理注册群信息
 * @param[in] response 成功结果
 */
void PanelGroupPage::recvRegistGroup(RegistGroupResponse response)
{
    if(response.userId == G_User->BaseInfo().uuid){
        emit registChatGroupResult(true);
    }else{
        emit registChatGroupResult(false);
    }
}

void PanelGroupPage::recvRegistGroupFailed()
{
    emit registChatGroupResult(false);
}

/*!
 * @brief LineEdit中命名完成后将内容设置为page的name
 * @param[in] 无
 * @return 无
 */
void PanelGroupPage::renameEditFinished()
{
    MQ_D(PanelGroupPage);
    if(d->tmpNameEdit->text().size() > 0 && d->tmpNameEdit->text() != d->pageNameBeforeRename)
    {
        d->toolBox->selectedPage()->setToolName(d->tmpNameEdit->text());

        GroupingRequest * request = new GroupingRequest();
        request->uuid = G_User->BaseInfo().uuid;
        if(d->groupIsCreate){
            request->type = GROUPING_CREATE;
        }else{
            request->type = GROUPING_RENAME;
        }
        request->gtype = OperateGroup;
        request->groupName = d->tmpNameEdit->text();
        request->groupId = d->toolBox->selectedPage()->getID();

        RChatGroupData * tmpData = new RChatGroupData;
        tmpData->groupId = request->groupId;
        tmpData->groupName = request->groupName;
        tmpData->isDefault = false;
        RSingleton<UserChatContainer>::instance()->addTmpGroup(request->groupId,tmpData);

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
 */
void PanelGroupPage::updateGroupActions(ToolPage * page)
{
    MQ_D(PanelGroupPage);
    d->pageOfMovedItem = page;
    QList <PersonGroupInfo> infoList = d->toolBox->toolPagesinfos();
    QMenu * moveGroupTo  = ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLITEM_GROUPS);
    if(!moveGroupTo->isEmpty())
        moveGroupTo->clear();

    for(int index=0;index<infoList.count();index++)
    {
        if(page->pageInfo().uuid != infoList.at(index).uuid){
            QAction *tmpAction = new QAction(infoList.at(index).name);
            tmpAction->setData(infoList.at(index).uuid);
            connect(tmpAction,SIGNAL(triggered(bool)),this,SLOT(moveGroupTo()));
            moveGroupTo->addAction(tmpAction);
        }else{
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
    QString t_targetUuid = target->data().toString();
    ToolPage * t_targetPage = d->toolBox->targetPage(t_targetUuid);
    ToolPage * t_sourcePage = d->pageOfMovedItem;
    d->m_movedItem = d->toolBox->selectedItem();
    if(!t_targetPage||!t_sourcePage||!d->m_movedItem)
        return;

    GroupingFriendRequest * request = new GroupingFriendRequest;
    request->type = G_Friend_MOVE;
    request->stype = OperateGroup;
    request->groupId = t_targetPage->getID();
    request->oldGroupId = t_sourcePage->getID();

    UserClient * client = RSingleton<UserManager>::instance()->client(d->m_movedItem);
    if(client)
        request->user = client->simpleUserInfo;

    RSingleton<MsgWrap>::instance()->handleMsg(request);
}

void PanelGroupPage::createAction()
{
    MQ_D(PanelGroupPage);

    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLBOX);

    QAction * searchGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_SEARCHGROUP,this,SLOT(searchGroup()));
    searchGroupAction->setText(tr("Search group"));

    QAction * newGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_ADDGROUP,this,SLOT(createNewGroup()));
    newGroupAction->setText(tr("New group"));

    QAction * addGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_ADDGROUPS,this,SLOT(respGroupCreate()));
    addGroupAction->setText(tr("Add groups"));

    menu->addAction(searchGroupAction);
    menu->addAction(newGroupAction);
    menu->addSeparator();
    menu->addAction(addGroupAction);

    d->toolBox->setContextMenu(menu);

    //创建分组的右键菜单
    QMenu * groupMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLGROUP);

    QAction * renameAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_RENAME,this,SLOT(respGroupRename()));
    renameAction->setText(tr("Rename group"));

    QAction * delGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_DELGROUP,this,SLOT(respGroupDeleted()));
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

/*!
 * @brief 创建分组列表
 * @details 接收数据后，1.创建对应的ToolItem；2.更新对应Client的基本信息。
 */
void PanelGroupPage::createGroup()
{
    MQ_D(PanelGroupPage);

    for(int i = 0; i < RSingleton<UserChatContainer>::instance()->groupSize();i++)
    {
        RChatGroupData * groupData = RSingleton<UserChatContainer>::instance()->element(i);
        if(groupData){
            ToolPage * page = d->toolBox->addPage(groupData->groupName);
            page->setID(groupData->groupId);
            page->setDefault(groupData->isDefault);

            for(int j = 0; j < groupData->chatGroups.size(); j++)
            {
                SimpleChatInfo * userInfo = groupData->chatGroups.at(j);

                ToolItem * item = ceateItem(userInfo,page);
                page->addItem(item);
            }
            updateGroupDescInfo(page);

            page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLGROUP));
        }
    }

    d->isGroupHasCreated = true;
}

/*!
 * @brief 创建单元Item
 * @param[in] userInfo 用户基本信息
 * @param[in] page 所属页面
 * @return ToolItem * 创建后Item
 */
ToolItem * PanelGroupPage::ceateItem(SimpleChatInfo * chatInfo,ToolPage * page)
{
    ToolItem * item = new ToolItem(page);
    connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
//    connect(item,SIGNAL(showChatWindow(ToolItem*)),this,SLOT(createChatWindow(ToolItem*)));
//    connect(item,SIGNAL(itemDoubleClick(ToolItem*)),this,SLOT(showChatWindow(ToolItem*)));
//    connect(item,SIGNAL(itemMouseHover(bool,ToolItem*)),MainDialog::instance(),SLOT(showHoverItem(bool,ToolItem*)));
    connect(item,SIGNAL(updateGroupActions()),page,SLOT(updateGroupActions()));

    item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLITEM));
    item->setName(chatInfo->remarks);

    GroupClient * client = RSingleton<GroupManager>::instance()->addClient(chatInfo->chatId);
    client->simpleChatInfo = *chatInfo;
    client->toolItem = item;
    return item;
}

/*!
 * @brief 更新分组人数信息
 * @details 在添加联系人、删除联系人、联系人状态改变、移动联系人等均调用
 * @param[in] page 待更新的分组
 */
void PanelGroupPage::updateGroupDescInfo(ToolPage *page)
{
    page->setDescInfo(QString("%1").arg(page->items().size()));
}

void PanelGroupPage::updateGroupDescInfo()
{
    MQ_D(PanelGroupPage);
    std::for_each(d->toolBox->allPages().begin(),d->toolBox->allPages().end(),
                  [&](ToolPage *page){this->updateGroupDescInfo(page);});
}

/*!
 * @brief 接收到服务器删除分组成功后更新群分组显示
 * @param[in] id:QString，待删除的群分组id
 * @return 无
 */
void PanelGroupPage::clearTargetGroup(const QString id)
{
    MQ_D(PanelGroupPage);
    QString t_id = id;
    ToolPage * t_delPage = d->toolBox->targetPage(t_id);
    if(!t_delPage)
        return;

    foreach(ToolItem *t_movedItem,t_delPage->items()){
        bool t_result = t_delPage->removeItem(t_movedItem);
        if(t_result)
            d->toolBox->defaultPage()->addItem(t_movedItem);
        else
            delete t_movedItem;
    }

    //[2]更新列表
    RSingleton<UserChatContainer>::instance()->deleteGroup(id);

    d->toolBox->removePage(t_delPage);
    d->toolBox->removeFromList(t_delPage);
    d->m_deleteID = QString();
}

PanelGroupPage::~PanelGroupPage()
{

}
