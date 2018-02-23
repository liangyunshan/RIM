#include "panelpersonpage.h"

#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLineEdit>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "maindialog.h"
#include "global.h"
#include "actionmanager/actionmanager.h"
#include "rsingleton.h"
#include "Network/msgwrap.h"
#include "user/userclient.h"
#include "messdiapatch.h"
#include "maindialog.h"

#include "toolbox/toolbox.h"
using namespace ProtocolType;

using namespace GroupPerson;

class PanelPersonPagePrivate : public GlobalData<PanelPersonPage>
{
    Q_DECLARE_PUBLIC(PanelPersonPage)
protected:
    PanelPersonPagePrivate(PanelPersonPage * q):
        q_ptr(q)
    {
        groupIsCreate = false;
        initWidget();
    }

    void initWidget();

    PanelPersonPage * q_ptr;
    ToolBox * toolBox;

    QWidget * contentWidget;
    QLineEdit *tmpNameEdit;                         //重命名时用edit
    ToolPage * pageOfMovedItem;                     //待移动分组的item所属的page
    ToolItem * m_movedItem;                         //待移动的联系人item
    bool groupIsCreate;                             //标识分组是新创建还是已存在
    QString m_deleteID;                             //暂时将删除的分组ID保存在内存中

    QList<ToolPage *> pages;
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

    pageOfMovedItem = NULL;
    m_movedItem = NULL;
}

PanelPersonPage::PanelPersonPage(QWidget *parent):
    d_ptr(new PanelPersonPagePrivate(this)),
    QWidget(parent)
{
    createAction();

    connect(this,SIGNAL(showChatDialog(ToolItem*)),MainDialog::instance(),SLOT(showChatWindow(ToolItem*)));
    connect(MessDiapatch::instance(),SIGNAL(recvRelationFriend(MsgOperateResponse,GroupingFriendResponse)),this,SLOT(recvRelationFriend(MsgOperateResponse,GroupingFriendResponse)));

    RSingleton<Subject>::instance()->attach(this);
}

PanelPersonPage::~PanelPersonPage()
{
    RSingleton<Subject>::instance()->detach(this);
}

//更新好友列表
void PanelPersonPage::addGroupAndUsers()
{
    MQ_D(PanelPersonPage);
    for(int i = 0; i < G_FriendList.size();i++)
    {
        RGroupData * groupData = G_FriendList.at(i);
        ToolPage * page = d->toolBox->addPage(groupData->groupName);
        page->setID(groupData->groupId);
        page->setDefault(groupData->isDefault);

        for(int j = 0; j < groupData->users.size(); j++)
        {
            SimpleUserInfo userInfo = groupData->users.at(j);

            ToolItem * item = ceateItem(userInfo,page);
            page->addItem(item);
        }

        page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
    }
}

/*!
     * @brief 接收到服务器删除分组成功后更新联系人分组显示
     * @param[in] id:QString，待删除的联系人分组id
     * @return 无
     */
void PanelPersonPage::clearTargetGroup(const QString id)
{
    MQ_D(PanelPersonPage);
    QString t_id = id;
    ToolPage * t_delPage = d->toolBox->targetPage(t_id);
    if(!t_delPage)
    {
        return;
    }
    foreach(ToolItem *t_movedItem,t_delPage->items())
    {
        bool t_result = t_delPage->removeItem(t_movedItem);
        if(t_result)
        {
            d->toolBox->defaultPage()->addItem(t_movedItem);
        }
        else
        {
            delete t_movedItem;
        }
    }
    d->toolBox->removePage(t_delPage);
    d->toolBox->removeFromList(t_delPage);//FIXME 删除列表中的page且收回内存
    d->m_deleteID = QString();
}

/*!
     * @brief 接收服务器的消息更新联系人分组在线联系人数目
     * @param[in] info:SimpleUserInfo &，待更新的联系人分组id
     * @return 无
     */
void PanelPersonPage::updateContactShow(const SimpleUserInfo & info)
{
    ToolItem * t_item = RSingleton<UserManager>::instance()->client(info.accountId)->toolItem;
    if(!t_item)
    {
        return;
    }
    t_item->setName(info.remarks);
    t_item->setNickName(info.nickName);
    t_item->setDescInfo(info.signName);
    t_item->setStatus(info.status);
}

void PanelPersonPage::onMessage(MessageType type)
{
    MQ_D(PanelPersonPage);
    switch(type)
    {
        case MESS_FRIENDLIST_UPDATE:
            addGroupAndUsers();
            break;
        case MESS_GROUP_DELETE:
            clearTargetGroup(d->m_deleteID);
            break;
        default:
            break;
    }
}

/*!
     * @brief 刷新当前用户列表，重新发送请求至服务器
     * @param[in] 无
     * @return 无
     */
void PanelPersonPage::refreshList()
{
//    MQ_D(PanelPersonPage);
//    d->m_deleteID = t_page->id();
//    GroupingRequest * request = new GroupingRequest();
//    request->uuid = G_UserBaseInfo.uuid;
//    request->type = GROUPING_REFRESH;
//    request->gtype = GROUPING_FRIEND;
//    request->groupId = t_page->id();

//    RSingleton<MsgWrap>::instance()->handleMsg(request);
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
    MQ_D(PanelPersonPage);
    d->groupIsCreate = true;
    ToolPage * page = d->toolBox->addPage(QStringLiteral("untitled"));
    page->setDefault(false);
    page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
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
     * @param[in] 无
     * @return 无
     *
     */
void PanelPersonPage::renameGroup()
{
    MQ_D(PanelPersonPage);
    ToolPage * page = d->toolBox->selectedPage();
    if(page)
    {
        d->groupIsCreate = false;

        QRect textRec = page->textRect();
        QRect pageRec = page->geometry();
        d->tmpNameEdit->raise();
        d->tmpNameEdit->setText(page->toolName());
        d->tmpNameEdit->selectAll();
        d->tmpNameEdit->setGeometry(textRec.x(),pageRec.y(),pageRec.width(),textRec.height());
        d->tmpNameEdit->show();
        d->tmpNameEdit->setFocus();
    }
}

void PanelPersonPage::delGroup()
{
    MQ_D(PanelPersonPage);
    ToolPage *t_page = d->toolBox->selectedPage();
    if(t_page->isDefault())
    {
        return;
    }
    d->m_deleteID = t_page->getID();
    GroupingRequest * request = new GroupingRequest();
    request->uuid = G_UserBaseInfo.uuid;
    request->type = GROUPING_DELETE;
    request->gtype = GROUPING_FRIEND;
    request->groupId = t_page->getID();

    RSingleton<MsgWrap>::instance()->handleMsg(request);
}

void PanelPersonPage::createChatWindow(ToolItem *item)
{
    Q_UNUSED(item);
}

void PanelPersonPage::sendInstantMessage()
{
    MQ_D(PanelPersonPage);
    emit showChatDialog(d->toolBox->selectedItem());
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

/*!
     * @brief 接收分组好友操作结果
     * @param[in] response 结果信息
     * @return 无
     */
void PanelPersonPage::recvRelationFriend(MsgOperateResponse result, GroupingFriendResponse response)
{
    MQ_D(PanelPersonPage);
    switch(response.type)
    {
    case G_Friend_CREATE:
        {
            if(result == STATUS_SUCCESS)
            {
                QList<ToolPage *>::iterator iter = d->toolBox->allPages().begin();

                while(iter != d->toolBox->allPages().end())
                {
                    if((*iter)->getID() == response.groupId)
                    {
                        ToolItem * item = ceateItem(response.user,(*iter));
                        (*iter)->addItem(item);
                        break;
                    }
                    iter++;
                }

                QList<RGroupData *>::iterator groupIter =  G_FriendList.begin();
                while(groupIter != G_FriendList.end())
                {
                    if((*groupIter)->groupId == response.groupId)
                    {
                        (*groupIter)->users.append(response.user);
                        break;
                    }
                    groupIter++;
                }
                //1.查找用户界面需要判断此用户是否已经被添加了
                RSingleton<Subject>::instance()->notify(MESS_RELATION_FRIEND_ADD);
            }
            else
            {

            }
            break;
        }
    case G_Friend_UPDATE:
        {
            break;
        }
    case G_Friend_MOVE:
        {
            if(result == STATUS_SUCCESS)
            {
                QString t_sourceID = response.oldGroupId;
                QString t_targetID = response.groupId;
                ToolPage * t_sourcePage = d->toolBox->targetPage(t_sourceID);
                ToolPage * t_targetPage = d->toolBox->targetPage(t_targetID);
                bool t_rmResult = t_sourcePage->removeItem(d->m_movedItem);
                if(t_rmResult)
                {
                    d->toolBox->targetPage(t_targetID)->addItem(d->m_movedItem);
                    disconnect(d->m_movedItem,SIGNAL(updateGroupActions()),t_sourcePage,SLOT(updateGroupActions()));
                    connect(d->m_movedItem,SIGNAL(updateGroupActions()),t_targetPage,SLOT(updateGroupActions()));
                }
            }
            else
            {

            }
            break;
        }
    default:
        break;
    }
}

ToolItem * PanelPersonPage::ceateItem(SimpleUserInfo & userInfo,ToolPage * page)
{
    ToolItem * item = new ToolItem(page);
    connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
    connect(item,SIGNAL(showChatWindow(ToolItem*)),this,SLOT(createChatWindow(ToolItem*)));
    connect(item,SIGNAL(itemDoubleClick(ToolItem*)),MainDialog::instance(),SLOT(showChatWindow(ToolItem*)));
    connect(item,SIGNAL(itemMouseHover(bool,ToolItem*)),MainDialog::instance(),SLOT(showHoverItem(bool,ToolItem*)));
    connect(item,SIGNAL(updateGroupActions()),page,SLOT(updateGroupActions()));

    item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM));
    item->setName(userInfo.remarks);
    item->setNickName(userInfo.nickName);
    item->setDescInfo(userInfo.signName);

    UserClient * client = RSingleton<UserManager>::instance()->addClient(userInfo.accountId);
    client->simpleUserInfo = userInfo;
    client->toolItem = item;

    return item;
}

/*!
     * @brief LineEdit中命名完成后将内容设置为page的name
     * @param[in] 无
     * @return 无
     */
void PanelPersonPage::renameEditFinished()
{
    MQ_D(PanelPersonPage);
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
        request->gtype = GROUPING_FRIEND;
        request->groupName = d->tmpNameEdit->text();
        request->groupId = d->toolBox->selectedPage()->getID();
        RSingleton<MsgWrap>::instance()->handleMsg(request);

        d->groupIsCreate = true;
    }
    d->tmpNameEdit->setText("");
    d->tmpNameEdit->hide();
}

/*!
     * @brief 根据触发右键菜单的Item所属的Page来添加“移动联系人至”菜单中Action
     * @param[in] page：ToolPage *
     * @return 无
     *
     */
void PanelPersonPage::updateGroupActions(ToolPage * page)
{
    MQ_D(PanelPersonPage);
    d->pageOfMovedItem = page;
    QList <PersonGroupInfo> infoList = d->toolBox->toolPagesinfos();
    QMenu * movePersonTo  = ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM_GROUPS);
    if(!movePersonTo->isEmpty())
    {
        movePersonTo->clear();
    }
    for(int index=0;index<infoList.count();index++)
    {
        if(page->pageInfo().uuid != infoList.at(index).uuid)
        {
            QAction *tmpAction = new QAction(infoList.at(index).name);
            tmpAction->setData(infoList.at(index).uuid);
            connect(tmpAction,SIGNAL(triggered(bool)),this,SLOT(movePersonTo()));
            movePersonTo->addAction(tmpAction);
        }
        else
        {
            continue;
        }
    }
}

/*!
     * @brief 移动至各分组的Action响应
     * @details 只处理移动的请求，待服务器移动成功后，再真实的移动
     * @param
     * @return 无
     */
void PanelPersonPage::movePersonTo()
{
    MQ_D(PanelPersonPage);

    QAction * target = qobject_cast<QAction *>(QObject::sender());
    QString t_targetUuid = target->data().toString();
    ToolPage * t_targetPage = d->toolBox->targetPage(t_targetUuid);
    ToolPage * t_sourcePage = d->pageOfMovedItem;
    d->m_movedItem = d->toolBox->selectedItem();
    if(!t_targetPage||!t_sourcePage||!d->m_movedItem)
    {
        return;
    }
    else
    {
        GroupingFriendRequest * request = new GroupingFriendRequest;
        request->type = G_Friend_MOVE;
        request->stype = SearchPerson;
        request->groupId = t_targetPage->getID();
        request->oldGroupId = t_sourcePage->getID();

        UserClient * client = RSingleton<UserManager>::instance()->client(d->m_movedItem);
        if(client)
        {
            request->user = client->simpleUserInfo;
        }
        RSingleton<MsgWrap>::instance()->handleMsg(request);
    }
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

    QAction * movePersonTOAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_MOVEPERSON);
    movePersonTOAction->setText(tr("Move Friend To"));

    QMenu * groupsMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLITEM_GROUPS);
    movePersonTOAction->setMenu(groupsMenu);

    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_SENDMESSAGE));
    personMenu->addSeparator();
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_VIEWDETAIL));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MODIFYCOMMENTS));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_DELPERSON));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MOVEPERSON));
    personMenu->addSeparator();
}
