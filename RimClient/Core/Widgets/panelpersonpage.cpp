#include "panelpersonpage.h"

#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <algorithm>

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
#include "modifyremarkwindow.h"
#include "contactdetailwindow.h"
#include "user/user.h"
#include "user/userfriendcontainer.h"

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
        m_modifyWindow = NULL;
        m_detailWindow = NULL;
        groupIsCreate = false;
        m_listIsCreated = false;
        initWidget();
    }

    void initWidget();

    PanelPersonPage * q_ptr;
    ToolBox * toolBox;

    QWidget * contentWidget;
    QLineEdit *tmpNameEdit;                         //重命名时用edit
    ToolPage * pageOfTriggeredItem;                 //待移动分组的item所属的page
    ToolItem * m_movedItem;                         //待移动的联系人item
    bool groupIsCreate;                             //标识分组是新创建还是已存在
    QString m_deleteID;                             //暂时将删除的分组ID保存在内存中
    bool m_listIsCreated;                           //标识好友列表是第一次创建还是刷新显示

    ModifyRemarkWindow *m_modifyWindow;
    ContactDetailWindow *m_detailWindow;

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

    pageOfTriggeredItem = NULL;
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

/*!
 * @brief 创建好友列表
 * @details 接收数据后，1.创建对应的ToolItem；2.更新对应Client的基本信息。
 */
void PanelPersonPage::addGroupAndUsers()
{
    MQ_D(PanelPersonPage);

    for(int i = 0; i < RSingleton<UserFriendContainer>::instance()->groupSize();i++)
    {
        RGroupData * groupData = RSingleton<UserFriendContainer>::instance()->element(i);
        if(groupData){
            ToolPage * page = d->toolBox->addPage(groupData->groupName);
            page->setID(groupData->groupId);
            page->setDefault(groupData->isDefault);

            for(int j = 0; j < groupData->users.size(); j++)
            {
                SimpleUserInfo * userInfo = groupData->users.at(j);

                ToolItem * item = ceateItem(userInfo,page);
                page->addItem(item);
            }
            updateGroupDescInfo(page);

            page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
        }
    }

    d->m_listIsCreated = true;
}

/*!
 * @brief 更新分组人数信息
 * @details 在添加联系人、删除联系人、联系人状态改变、移动联系人等均调用
 * @param[in] page 待更新的分组
 */
void PanelPersonPage::updateGroupDescInfo(ToolPage *page)
{
    int onlineCount = 0;
    std::count_if(page->items().cbegin(),page->items().cend(),[&onlineCount](const ToolItem *info){
        if(info->isOnline())
            onlineCount++;
        return true;
    });

    page->setDescInfo(QString("%1/%2").arg(onlineCount).arg(page->items().size()));
}

void PanelPersonPage::updateGroupDescInfo()
{
    MQ_D(PanelPersonPage);
    std::for_each(d->toolBox->allPages().begin(),d->toolBox->allPages().end(),
                  [&](ToolPage *page){this->updateGroupDescInfo(page);});
}

/*!
 * @brief 接收到服务器删除分组成功后更新联系人分组显示
 * @details 若此分组中包含联系人，则将联系人移动至默认分组；
 * @attention 客户都在接收到正确的删除结果信息时，服务器已经将待删除的联系人移动至默认分组，因此客户端直接移动即可。
 * @param[in] id:QString，待删除的联系人分组id
 * @return 无
 */
void PanelPersonPage::clearTargetGroup(const QString id)
{
    MQ_D(PanelPersonPage);
    QString t_id = id;
    ToolPage * t_delPage = d->toolBox->targetPage(t_id);
    if(!t_delPage){
        return;
    }
    //[1]将分组下的联系人移动至默认分组
    foreach(ToolItem *t_movedItem,t_delPage->items()){
        bool t_result = t_delPage->removeItem(t_movedItem);
        if(t_result){
            d->toolBox->defaultPage()->addItem(t_movedItem);
        }else{
            delete t_movedItem;
        }
    }

    //[2]更新列表
    RSingleton<UserFriendContainer>::instance()->deleteGroup(id);

    d->toolBox->removePage(t_delPage);
    d->toolBox->removeFromList(t_delPage);  //FIXME 删除列表中的page且收回内存
    d->m_deleteID = QString();
}

/*!
 * @brief 接收服务器的消息更新联系人分组在线联系人数目
 * @param[in] info:SimpleUserInfo &，待更新的联系人信息
 * @return 无
 */
void PanelPersonPage::updateContactShow(const SimpleUserInfo & info)
{
    ToolItem * t_item = RSingleton<UserManager>::instance()->client(info.accountId)->toolItem;
    if(!t_item){
        return;
    }
    t_item->setName(info.remarks);
    t_item->setNickName(info.nickName);
    t_item->setDescInfo(info.signName);
    t_item->setStatus(info.status);
}

/*!
 * @brief 接收服务器的消息删除某个分组中的联系人
 * @attention 从当前列表中移除好友的同时还需从全局好友列表中将此人此账户移除，否则再次添加时不可用。
 * @param[in] info:SimpleUserInfo &，待更新的联系人信息
 * @return 无
 */
void PanelPersonPage::removeContact(const SimpleUserInfo & info)
{
    MQ_D(PanelPersonPage);
    UserClient * userClient = RSingleton<UserManager>::instance()->client(info.accountId);
    if(userClient)
    {
        ToolItem * t_item = userClient->toolItem;
        if(!t_item)
        {
            return;
        }
        ToolPage * pageItem = d->toolBox->targetPage(t_item);
        if(pageItem)
        {
            bool t_removeResult = pageItem->removeItem(t_item);
            if(t_removeResult)
            {
                bool t_result = RSingleton<UserManager>::instance()->removeClient(info.accountId);
                if(t_result)
                {
                    delete t_item;
                    RSingleton<UserFriendContainer>::instance()->deleteUser(pageItem->getID(),info.accountId);
                }
            }
        }
    }
}

/*!
 * @brief 清除本地存在但数据库中不存在的分组与联系人信息（可能有这种情况存在）
 * @param[in] 无
 * @return 无
 */
void PanelPersonPage::clearUnrealGroupAndUser()
{
    MQ_D(PanelPersonPage);
    QList<int> t_unrealPages;

    for(int t_pageIndex = 0;t_pageIndex < d->toolBox->allPages().count();t_pageIndex++)
    {
        ToolPage * t_tempPage = d->toolBox->allPages().at(t_pageIndex);
        QList<int>t_unrealItems;//保存本地存在但数据库中已经不存在的联系人索引值

        RGroupData * groupData = RSingleton<UserFriendContainer>::instance()->element(t_tempPage->getID());
        if(groupData){
            QList<SimpleUserInfo *> t_users = groupData->users;
            for(int t_itemIndex = 0;t_itemIndex<t_tempPage->items().count();t_tempPage++)
            {
                bool t_userMatchResult = false;
                for(int t_userIndex = 0;t_userIndex < t_users.count();t_userIndex++)
                {
                    ToolItem * t_curItem = t_tempPage->items().at(t_itemIndex);
                    UserClient * t_client = RSingleton<UserManager>::instance()->client(t_curItem);
                    if(t_client->simpleUserInfo.accountId == t_users.at(t_userIndex)->accountId)
                    {
                        t_userMatchResult = true;
                    }
                }
                if(!t_userMatchResult)
                {
                    t_unrealItems.append(t_itemIndex);
                }
            }
            for(int index = 0;index<t_unrealItems.count();index++)
            {
                ToolItem * temp = t_tempPage->items().at(t_unrealItems.at(index));
                bool t_removeResult = t_tempPage->removeItem(temp);
                if(t_removeResult)
                {
                    UserClient * userClient = RSingleton<UserManager>::instance()->client(temp);
                    bool t_result = RSingleton<UserManager>::instance()->removeClient(userClient->simpleUserInfo.accountId);
                    if(t_result)
                    {
                        delete temp;
                    }
                }
            }

            t_unrealPages.append(t_pageIndex);
        }
    }
    for(int t_index = 0;t_index < t_unrealPages.count();t_index++)
    {
        ToolPage * t_targetPage = d->toolBox->allPages().at(t_unrealPages.at(t_index));
        if(t_targetPage)
        {
            foreach(ToolItem *t_removedItem,t_targetPage->items())
            {
                bool t_itemResult = t_targetPage->removeItem(t_removedItem);
                if(t_itemResult)
                {
                    UserClient * t_client = RSingleton<UserManager>::instance()->client(t_removedItem);
                    bool t_clientResult = RSingleton<UserManager>::instance()->removeClient(t_client->simpleUserInfo.accountId);
                    if(t_clientResult)
                    {
                        delete t_removedItem;
                    }
                }
            }
            d->toolBox->removePage(t_targetPage);
            d->toolBox->removeFromList(t_targetPage);
        }
    }
}

void PanelPersonPage::onMessage(MessageType type)
{
    MQ_D(PanelPersonPage);
    switch(type)
    {
        case MESS_FRIENDLIST_UPDATE:
            if(!d->m_listIsCreated){
                addGroupAndUsers();
            }else{
                updateContactList();
            }
            break;

        case MESS_GROUP_DELETE:
            clearTargetGroup(d->m_deleteID);
            updateGroupDescInfo();
            break;

        case MESS_FRIEND_STATE_CHANGE:
            updateGroupDescInfo();
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
    GroupingRequest * request = new GroupingRequest();
    request->uuid = G_User->BaseInfo().uuid;
    request->type = GROUPING_REFRESH;
    request->gtype = GROUPING_FRIEND;

    RSingleton<MsgWrap>::instance()->handleMsg(request);
    //TODO 服务器添加代码处理客户端刷新联系人列表请求
}

/*!
 * @brief 添加新分组
 * @param[in] 无
 * @return 无
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
    request->uuid = G_User->BaseInfo().uuid;
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

/*!
     * @brief 显示好友资料
     * @param[in]
     * @return 无
     */
void PanelPersonPage::showUserDetail()
{
    MQ_D(PanelPersonPage);
    if(!d->m_detailWindow)
    {
        d->m_detailWindow = new ContactDetailWindow();
        connect(d->m_detailWindow,SIGNAL(destroyed(QObject*)),this,SLOT(updateDetailInstance(QObject*)));
    }

    if(d->m_detailWindow->isMinimized())
    {
        d->m_detailWindow->showNormal();
    }
    else
    {
        d->m_detailWindow->show();
    }
    UserClient * client = RSingleton<UserManager>::instance()->client(d->toolBox->selectedItem());
    if(client)
    {
        d->m_detailWindow->setContactDetail(client->simpleUserInfo);
    }

    QStringList t_groupNames;
    int t_currentGroup = -1;
    for(int t_index = 0;t_index<d->toolBox->pageCount();t_index++)
    {
        t_groupNames.append(d->toolBox->allPages().at(t_index)->toolName());
        if(d->pageOfTriggeredItem->toolName() == d->toolBox->allPages().at(t_index)->toolName())
        {
            t_currentGroup = t_index;
        }
    }
    d->m_detailWindow->setGroups(t_groupNames,t_currentGroup);
}

/*!
     * @brief 修改好友备注信息
     * @param[in]
     * @return 无
     */
void PanelPersonPage::modifyUserInfo()
{
    MQ_D(PanelPersonPage);
    ToolItem *t_modifyItem = d->toolBox->selectedItem();
    if(t_modifyItem && d->pageOfTriggeredItem)
    {
        if(!d->m_modifyWindow)
        {
            d->m_modifyWindow = new ModifyRemarkWindow();
            connect(d->m_modifyWindow,SIGNAL(setNewRemark(QString)),this,SLOT(requestModifyRemark(QString)));
            connect(d->m_modifyWindow,SIGNAL(destroyed(QObject*)),this,SLOT(updateModifyInstance(QObject*)));
        }

        if(d->m_modifyWindow->isMinimized())
        {
            d->m_modifyWindow->showNormal();
        }
        else
        {
            d->m_modifyWindow->show();
        }
        d->m_modifyWindow->setOldRemarkName(t_modifyItem->getName());
    }
}

/*!
 * @brief 请求服务器删除好友
 * @param[in]
 * @return 无
 */
void PanelPersonPage::deleteUser()
{
    MQ_D(PanelPersonPage);

    ToolItem *t_toRemovedItem = d->toolBox->selectedItem();
    if(t_toRemovedItem && d->pageOfTriggeredItem)
    {
        GroupingFriendRequest * t_request = new GroupingFriendRequest();
        t_request->type = G_Friend_Delete;
        t_request->stype = SearchPerson;
        t_request->groupId = d->pageOfTriggeredItem->getID();
        t_request->oldGroupId = d->pageOfTriggeredItem->getID();
        UserClient * client = RSingleton<UserManager>::instance()->client(t_toRemovedItem);
        if(client)
        {
            t_request->user = client->simpleUserInfo;
        }
        RSingleton<MsgWrap>::instance()->handleMsg(t_request);
    }

}

/*!
 * @brief 接收分组好友操作结果
 * @details 处理的类型包括1.创建好友；2.更新好友信息；3.移动好友；4.删除好友
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
                if(!RSingleton<UserFriendContainer>::instance()->containUser(response.user.accountId)){
                    QList<ToolPage *>::iterator iter = d->toolBox->allPages().begin();

                    while(iter != d->toolBox->allPages().end())
                    {
                        if((*iter)->getID() == response.groupId)
                        {
                            ToolItem * item = ceateItem(&(response.user),(*iter));
                            (*iter)->addItem(item);
                            break;
                        }
                        iter++;
                    }
                    updateGroupDescInfo();

                    RSingleton<UserFriendContainer>::instance()->addUser(response.groupId,response.user);

                    //1.查找用户界面需要判断此用户是否已经被添加了
                    RSingleton<Subject>::instance()->notify(MESS_RELATION_FRIEND_ADD);
                }
            }
            else
            {
                //TODO 20180417添加好友失败
            }
            break;
        }
    case G_Friend_UPDATE:
        {
            if(result == STATUS_SUCCESS)
            {
               updateContactShow(response.user);
            }else{

            }
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
                if(t_rmResult){
                    d->toolBox->targetPage(t_targetID)->addItem(d->m_movedItem);
                    disconnect(d->m_movedItem,SIGNAL(updateGroupActions()),t_sourcePage,SLOT(updateGroupActions()));
                    connect(d->m_movedItem,SIGNAL(updateGroupActions()),t_targetPage,SLOT(updateGroupActions()));
                    updateGroupDescInfo();
                }
            }else{

            }
            break;
        }
    case G_Friend_Delete:
        {
             if(result == STATUS_SUCCESS)
             {
                 removeContact(response.user);
                 updateGroupDescInfo();
             }
        }
    default:
        break;
    }
}

void PanelPersonPage::updateModifyInstance(QObject *)
{
    MQ_D(PanelPersonPage);
    if(d->m_modifyWindow)
    {
        d->m_modifyWindow = NULL;
    }
}

/*!
 * @brief 发送备注更新请求
 * @param[in] remark 备注信息
 * @return 无
 */
void PanelPersonPage::requestModifyRemark(QString remark)
{
    MQ_D(PanelPersonPage);

    ToolItem *t_modifyItem = d->toolBox->selectedItem();
    if(t_modifyItem && d->pageOfTriggeredItem && remark != t_modifyItem->getName())
    {
        GroupingFriendRequest * t_request = new GroupingFriendRequest();
        t_request->type = G_Friend_UPDATE;
        t_request->stype = SearchPerson;
        t_request->groupId = d->pageOfTriggeredItem->getID();
        t_request->oldGroupId = d->pageOfTriggeredItem->getID();
        UserClient * client = RSingleton<UserManager>::instance()->client(t_modifyItem);
        if(client)
        {
            SimpleUserInfo t_changedInfo = client->simpleUserInfo;
            t_changedInfo.remarks = remark;

            t_request->user = t_changedInfo;
        }
        else
        {
            t_request->user = client->simpleUserInfo;
        }
        RSingleton<MsgWrap>::instance()->handleMsg(t_request);
    }
}

void PanelPersonPage::updateDetailInstance(QObject *)
{
    MQ_D(PanelPersonPage);
    if(d->m_detailWindow)
    {
        d->m_detailWindow = NULL;
    }
}

/*!
 * @brief 在更新并收到数据更新通知后，刷新联系人列表
 * @param[in] 无
 * @return 无
 */
void PanelPersonPage::updateContactList()
{
    MQ_D(PanelPersonPage);
    const QList<RGroupData *> list = RSingleton<UserFriendContainer>::instance()->list();
    for(int t_groupIndex = 0;t_groupIndex < list.count();t_groupIndex++)
    {
        //更新列表中page
        QString t_groupId = list.at(t_groupIndex)->groupId;
        bool t_matchGroupResult = false;
        for(int t_pageIndex = 0;t_pageIndex < d->toolBox->allPages().count();t_pageIndex++)
        {
            ToolPage * t_tempPage = d->toolBox->allPages().at(t_pageIndex);
            if(t_tempPage->getID() == t_groupId)
            {
                t_matchGroupResult = true;
                break;
            }
        }
        ToolPage * t_targetPage = d->toolBox->targetPage(t_groupId);
        if(t_matchGroupResult)
        {
            //本地有与回复信息中匹配的分组
            t_targetPage->setToolName(list.at(t_groupIndex)->groupName);
            //更新分组中item
            QList<SimpleUserInfo *> t_users = list.at(t_groupIndex)->users;
            for(int t_userIndex = 0;t_userIndex < t_users.count();t_userIndex++)
            {
                QString t_userId = t_users.at(t_userIndex)->accountId;
                bool t_matchUserResult = false;
                for(int t_itemIndex = 0;t_itemIndex < t_targetPage->items().count();t_targetPage++)
                {
                    ToolItem * t_tempItem = t_targetPage->items().at(t_itemIndex);
                    UserClient * t_client = RSingleton<UserManager>::instance()->client(t_tempItem);
                    if(t_client->baseInfo.accountId == t_userId)
                    {
                        t_matchUserResult = true;
                        break;
                    }
                }

                if(t_matchUserResult)       //数据库分组中有，且相同id的本地分组中也有目标联系人
                {
                    ToolItem * t_matchedItem = RSingleton<UserManager>::instance()->client(t_userId)->toolItem;
                    t_matchedItem->setName(t_users.at(t_userIndex)->remarks);
                    t_matchedItem->setNickName(t_users.at(t_userIndex)->nickName);
                    t_matchedItem->setDescInfo(t_users.at(t_userIndex)->signName);
                    t_matchedItem->setStatus(t_users.at(t_userIndex)->status);
                }
                else                        //数据库分组中有但相同id的本地分组中没有目标联系人
                {
                    ToolItem * t_newItem = ceateItem(t_users.at(t_userIndex),t_targetPage);
                    t_targetPage->addItem(t_newItem);
                }
            }

            updateGroupDescInfo(t_targetPage);
        }
        else
        {
            //数据库中存在但本地不存在的分组，则在本地新增分组
            RGroupData * t_groupData = list.at(t_groupIndex);
            ToolPage * t_newPage = d->toolBox->addPage(t_groupData->groupName);
            t_newPage->setID(t_groupData->groupId);
            t_newPage->setDefault(t_groupData->isDefault);

            for(int j = 0; j < t_groupData->users.size(); j++)
            {
                SimpleUserInfo * t_userInfo = t_groupData->users.at(j);

                ToolItem * t_item = ceateItem(t_userInfo,t_newPage);
                t_newPage->addItem(t_item);
            }
            updateGroupDescInfo(t_newPage);
            t_newPage->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
        }
    }
    //清除本地存在但数据库中已经不存在的page和item
    clearUnrealGroupAndUser();
}

/*!
 * @brief 创建单元Item
 * @param[in] userInfo 用户基本信息
 * @param[in] page 所属页面
 * @return ToolItem * 创建后Item
 */
ToolItem * PanelPersonPage::ceateItem(SimpleUserInfo * userInfo,ToolPage * page)
{
    ToolItem * item = new ToolItem(page);
    connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
    connect(item,SIGNAL(showChatWindow(ToolItem*)),this,SLOT(createChatWindow(ToolItem*)));
    connect(item,SIGNAL(itemDoubleClick(ToolItem*)),MainDialog::instance(),SLOT(showChatWindow(ToolItem*)));
    connect(item,SIGNAL(itemMouseHover(bool,ToolItem*)),MainDialog::instance(),SLOT(showHoverItem(bool,ToolItem*)));
    connect(item,SIGNAL(updateGroupActions()),page,SLOT(updateGroupActions()));

    item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM));
    item->setName(userInfo->remarks);
    item->setNickName(userInfo->nickName);
    item->setDescInfo(userInfo->signName);
    item->setStatus(userInfo->status);

    UserClient * client = RSingleton<UserManager>::instance()->addClient(userInfo->accountId);
    client->simpleUserInfo = *userInfo;
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
        request->uuid = G_User->BaseInfo().uuid;
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
 */
void PanelPersonPage::updateGroupActions(ToolPage * page)
{
    MQ_D(PanelPersonPage);
    d->pageOfTriggeredItem = page;
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
    ToolPage * t_sourcePage = d->pageOfTriggeredItem;
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
