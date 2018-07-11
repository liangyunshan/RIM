#include "panelpersonpage.h"

#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLineEdit>
#include <algorithm>
#include <QFileInfo>
#include <functional>
#include <QTimer>

#include "head.h"
#include "../protocol/datastruct.h"
#include "constants.h"
#include "maindialog.h"
#include "global.h"
#include "actionmanager/actionmanager.h"
#include "util/rsingleton.h"
#include "Network/msgwrap/wrapfactory.h"
#include "user/userclient.h"
#include "messdiapatch.h"
#include "maindialog.h"
#include "modifyremarkwindow.h"
#include "contactdetailwindow.h"
#include "user/user.h"
#include "user/userfriendcontainer.h"
#include "widget/rmessagebox.h"
#include "media/mediaplayer.h"
#include "thread/filerecvtask.h"
#include "Util/rutil.h"

#include "chatpersonwidget.h"

#include "toolbox/toolbox.h"
using namespace ProtocolType;

class PanelPersonPagePrivate : public GlobalData<PanelPersonPage>
{
    Q_DECLARE_PUBLIC(PanelPersonPage)
protected:
    PanelPersonPagePrivate(PanelPersonPage * q):
        q_ptr(q)
    {
        m_modifyWindow = NULL;
        groupIsCreate = false;
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
    QString pageNameBeforeRename;                   //重命名前分组名称，为了避免在步修改名称下依然更新服务器

    ModifyRemarkWindow *m_modifyWindow;

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
    QObject::connect(toolBox,SIGNAL(pageIndexMoved(int,int,QString)),q_ptr,SLOT(respGroupMoved(int,int,QString)));
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
    setAttribute(Qt::WA_DeleteOnClose);
    createAction();

    connect(this,SIGNAL(showChatDialog(ToolItem*)),this,SLOT(showChatWindow(ToolItem*)));
    connect(MessDiapatch::instance(),SIGNAL(recvFriendList(MsgOperateResponse,FriendListResponse*)),
            this,SLOT(updateFriendList(MsgOperateResponse,FriendListResponse*)));
    connect(MessDiapatch::instance(),SIGNAL(recvRelationFriend(MsgOperateResponse,GroupingFriendResponse)),this,SLOT(recvFriendItemOperate(MsgOperateResponse,GroupingFriendResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvFriendGroupingOperate(MsgOperateResponse,GroupingResponse)),
            this,SLOT(recvFriendPageOperate(MsgOperateResponse,GroupingResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvUserStateChangedResponse(MsgOperateResponse,UserStateResponse)),
            this,SLOT(recvUserStateChanged(MsgOperateResponse,UserStateResponse)));

    RSingleton<Subject>::instance()->attach(this);

#ifdef __LOCAL_CONTACT__
    updateFriendListAdapter();
#endif
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
void PanelPersonPage::removeTargetGroup(const QString id)
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
            t_movedItem->setToolPage(d->toolBox->defaultPage());
            reconnectItem(t_movedItem,t_delPage,d->toolBox->defaultPage());
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

    emit userInfoChanged(info.remarks,info.accountId);
}

/*!
 * @brief 接收服务器的消息删除某个分组中的联系人
 * @attention 从当前列表中移除好友的同时还需从全局好友列表中将此人此账户移除，否则再次添加时不可用。
 * @param[in] info:SimpleUserInfo &，待更新的联系人信息
 * @return 无
 */
void PanelPersonPage::removeContact(const QString accountId)
{
    MQ_D(PanelPersonPage);
    UserClient * userClient = RSingleton<UserManager>::instance()->client(accountId);
    if(userClient)
    {
        ToolItem * t_item = userClient->toolItem;
        if(!t_item){
            return;
        }
        ToolPage * pageItem = d->toolBox->targetPage(t_item);
        if(pageItem)
        {
            bool t_removeResult = pageItem->removeItem(t_item);
            if(t_removeResult)
            {
                bool t_result = RSingleton<UserManager>::instance()->removeClient(accountId);
                if(t_result)
                {
                    delete t_item;
                    RSingleton<UserFriendContainer>::instance()->deleteUser(pageItem->getID(),accountId);
                }
            }
        }
    }
}

void PanelPersonPage::onMessage(MessageType type)
{
    switch (type) {
    case MESS_TEXT_NET_ERROR:
            networkIsConnected(false);
        break;
    case MESS_USER_OFF_LINE:
            {
                std::function<void(UserClient*)> func = [](UserClient* item){
                    item->toolItem->setStatus(STATUS_OFFLINE);
                };
                RSingleton<UserManager>::instance()->for_each(func);

                updateGroupDescInfo();
            }
        break;
    default:
        break;
    }
}

#ifdef __LOCAL_CONTACT__
/*!
 * @brief 更新用户列表适配器，用于将本地用户列表按照规则转换成通用的用户列表
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void PanelPersonPage::updateFriendListAdapter()
{
    FriendListResponse * response = new FriendListResponse;
    response->type = REQUEST_FIRST;
    response->accountId = G_User->BaseInfo().accountId;

    RGroupData * groupData = new RGroupData;
    groupData->groupId = RUtil::UUID();
    groupData->groupName = QStringLiteral("我的好友");
    groupData->isDefault = true;
    groupData->index = 0;

    std::for_each(G_ParaSettings->outerNetConfig.begin(),G_ParaSettings->outerNetConfig.end(),[&groupData](const ParameterSettings::OuterNetConfig & user){
        SimpleUserInfo * userInfo = new SimpleUserInfo();
        userInfo->accountId = user.nodeId;
        userInfo->nickName = user.nodeId;
        userInfo->signName = "";
        userInfo->remarks = user.nodeId;
        userInfo->isSystemIcon = true;
        userInfo->iconId = "";
        userInfo->status = STATUS_ONLINE;
        groupData->users.append(userInfo);
    });

    response->groups.append(groupData);

    updateFriendList(STATUS_SUCCESS,response);
}
#endif

/*!
 * @brief 创建好友列表
 * @details 根据获取好友的列表，创建对应的分组信息，并设置基本的状态信息。
 * @param[in] friendList 好友列表
 * @return 无
 */
void PanelPersonPage::updateFriendList(MsgOperateResponse status,FriendListResponse *friendList)
{
    if(status == STATUS_SUCCESS){
        RSingleton<UserFriendContainer>::instance()->reset(friendList->groups);

        if(friendList->type == REQUEST_FIRST){
            addGroupAndUsers();
        }else if(friendList->type == REQUEST_RECONNECT || friendList->type == REQUEST_REFRESH){
            updateContactList();
        }
#ifndef __LOCAL_CONTACT__
        QTimer::singleShot(500,this,SLOT(loadHistoryMsg()));
#endif
    }else{
        RMessageBox::warning(this,QObject::tr("warning"),tr("Get friendList failed!"),RMessageBox::Yes);
    }
}

/*!
 * @brief 刷新当前用户列表，重新发送请求至服务器
 * @param[in] 无
 * @return 无
 */
void PanelPersonPage::refreshList()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;

    FriendListRequest * request = new FriendListRequest;
    request->type = REQUEST_REFRESH;
    request->accountId = G_User->BaseInfo().accountId;
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
}

/*!
 * @brief 添加新分组
 * @param[in] 无
 * @return 无
 */
void PanelPersonPage::respGroupCreate()
{
    MQ_D(PanelPersonPage);

    d->groupIsCreate = true;

    ToolPage * page = d->toolBox->addPage(QStringLiteral("untitled"));
    page->setDefault(false);
    page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));

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
 * @param[in] 无
 * @return 无
 */
void PanelPersonPage::respGroupRename()
{
    MQ_D(PanelPersonPage);
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

void PanelPersonPage::respGroupDeleted()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(PanelPersonPage);
    ToolPage *t_page = d->toolBox->selectedPage();
    if(t_page->isDefault())
        return;

    d->m_deleteID = t_page->getID();
    GroupingRequest * request = new GroupingRequest();
    request->uuid = G_User->BaseInfo().uuid;
    request->type = GROUPING_DELETE;
    request->gtype = OperatePerson;
    request->groupId = t_page->getID();

    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
}

/*!
 * @brief 响应分组移动
 * @param[in] newIndex 重新排序后，分组在列表中索引
 * @param[in] oldIndex 排序前，分组在列表中索引
 * @param[in] pageId 执行排序的分组ID
 */
void PanelPersonPage::respGroupMoved(int newIndex, int oldIndex, QString pageId)
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(PanelPersonPage);

    GroupingRequest * request = new GroupingRequest();
    request->uuid = G_User->BaseInfo().uuid;
    request->type = GROUPING_SORT;
    request->gtype = OperatePerson;
    request->groupName = d->tmpNameEdit->text();
    request->groupId = pageId;
    request->groupIndex = newIndex;

    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
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

void PanelPersonPage::showChatWindow(ToolItem * item)
{
    if(item == nullptr)
        return;

    UserClient * client = RSingleton<UserManager>::instance()->client(item);
    showOrCreateChatWindow(client);
}

/*!
 * @brief 创建来自历史会话操作的聊天对话框
 * @param[in] messType 信息类型，个人、群、系统消息
 * @param[in] accountId 对应消息账户ID
 */
void PanelPersonPage::showChatWindow(ChatMessageType messType, QString accountId)
{
    if(messType == CHAT_C2C){
        UserClient * client = RSingleton<UserManager>::instance()->client(accountId);
        showOrCreateChatWindow(client);
    }
}

void PanelPersonPage::showOrCreateChatWindow(UserClient *client)
{
    if(client == nullptr)
        return;

    if(client->chatPersonWidget)
    {
        client->chatPersonWidget->show();
    }
    else
    {
        ChatPersonWidget * widget = new ChatPersonWidget();
        widget->setUserInfo(client->simpleUserInfo);
#ifdef __LOCAL_CONTACT__
        widget->setOuterNetConfig(client->netConfig);
#endif
        widget->initChatRecord();
        client->chatPersonWidget = widget;
        widget->show();
    }
}

/*!
 * @brief 显示好友资料
 * @param[in]
 * @return 无
 */
void PanelPersonPage::showUserDetail()
{
    MQ_D(PanelPersonPage);

    UserClient * client = RSingleton<UserManager>::instance()->client(d->toolBox->selectedItem());
    if(client){
        createDetailView(client);
    }
}

/*!
 * @brief 显示联系人详细信息
 * @param[in] messtype 信息类型
 * @param[in] accountId 对应信息类型ID
 */
void PanelPersonPage::showUserDetail(ChatMessageType messtype, QString accountId)
{
    if(messtype == CHAT_C2C){
        UserClient * client = RSingleton<UserManager>::instance()->client(accountId);
        createDetailView(client);
    }
}

void PanelPersonPage::createDetailView(UserClient *client)
{
    if(client == nullptr)
        return;

    ContactDetailWindow * m_detailWindow = new ContactDetailWindow();
    m_detailWindow->setContactDetail(client->simpleUserInfo);

    QList<QPair<QString,QString>> groupNames = RSingleton<UserFriendContainer>::instance()->groupIdAndNames();
    QPair<QString,QString> gname = RSingleton<UserFriendContainer>::instance()->groupIdAndName(client->simpleUserInfo.accountId);

    QStringList groups,groupIds;
    std::for_each(groupNames.cbegin(),groupNames.cend(),[&groups,&groupIds](const QPair<QString,QString> groupName){
        groups<<groupName.second;
        groupIds<<groupName.first;
    });

    m_detailWindow->setGroups(groups,groupIds.indexOf(gname.first));
    m_detailWindow->show();
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

    ToolItem * t_toRemovedItem = d->toolBox->selectedItem();
    if(t_toRemovedItem && d->pageOfTriggeredItem){
        UserClient * client = RSingleton<UserManager>::instance()->client(t_toRemovedItem);
        if(client && d->pageOfTriggeredItem->getID().size() > 0){
            sendDeleteUserRequest(client,d->pageOfTriggeredItem->getID());
        }
    }
}

/*!
 * @brief 来自历史对话框的删除联系人请求
 * @param[in] messtype 信息类型
 * @param[in] accountId 对应信息类型ID
 */
void PanelPersonPage::deleteUser(ChatMessageType messtype, QString accountId)
{
    if(messtype == CHAT_C2C){
        QPair<QString,QString> gname = RSingleton<UserFriendContainer>::instance()->groupIdAndName(accountId);
        UserClient * client = RSingleton<UserManager>::instance()->client(accountId);
        if(client && gname.first.size() > 0){
            sendDeleteUserRequest(client,gname.first);
        }
    }
}

void PanelPersonPage::sendDeleteUserRequest(UserClient *client, QString groupId)
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    if(client == nullptr)
        return;
    GroupingFriendRequest * t_request = new GroupingFriendRequest();
    t_request->type = G_Friend_Delete;
    t_request->stype = OperatePerson;
    t_request->groupId = groupId;
    t_request->oldGroupId = groupId;
    t_request->user = client->simpleUserInfo;
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(t_request);
}

/*!
 * @brief 根据网络状态设置列表是否为灰色
 * @param[in] isConnected 网络是否连接
 */
void PanelPersonPage::networkIsConnected(bool isConnected)
{
    MQ_D(PanelPersonPage);
    if(isConnected){
        //TODO 查询在线联系人状态
    }else{
        for(int i = 0;i<d->toolBox->allPages().size();i++){
            ToolPage * page = d->toolBox->allPages().at(i);
            std::for_each(page->items().cbegin(),page->items().cend(),[](ToolItem *info){
                info->setStatus(STATUS_OFFLINE);
            });
        }
    }

    updateGroupDescInfo();
}

/*!
 * @brief 对当前列表重新排序刷新显示
 */
void PanelPersonPage::resortToolPage()
{
    MQ_D(PanelPersonPage);
    const QList<RGroupData *> list = RSingleton<UserFriendContainer>::instance()->list();

    int index = 0;
    std::for_each(list.begin(),list.end(),[&](RGroupData * groupData){
        d->toolBox->sortPage(groupData->groupId,index++);
    });
}

/*!
 * @brief 重新对在线用户和离线用户显示排序
 */
void PanelPersonPage::resortOnlineToolItem()
{

}

/*!
 * @brief 因删除、移动造成item所属父类改变，需要对item的信号进行重新关联
 * @param[in] item 待关联的item
 * @param[in] oldPage 旧Page
 * @param[in] newPage 新newPage
 */
void PanelPersonPage::reconnectItem(ToolItem *item, ToolPage *oldPage, ToolPage *newPage)
{
    disconnect(item,SIGNAL(updateGroupActions()),oldPage,SLOT(updateGroupActions()));
    disconnect(item,SIGNAL(clearSelectionOthers(ToolItem*)),oldPage,SIGNAL(clearItemSelection(ToolItem*)));
    connect(item,SIGNAL(updateGroupActions()),newPage,SLOT(updateGroupActions()));
    connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),newPage,SIGNAL(clearItemSelection(ToolItem*)));
}

/*!
 * @brief 接收分组好友操作结果
 * @details 处理的类型包括1.创建好友；2.更新好友信息；3.移动好友；4.删除好友
 *          【!!!!】同步更新UserFriendContainer【!!!!】
 * @param[in] response 结果信息
 * @return 无
 */
void PanelPersonPage::recvFriendItemOperate(MsgOperateResponse result, GroupingFriendResponse response)
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
            }else{
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
            if(result == STATUS_SUCCESS){
                QString sourceID = response.oldGroupId;
                QString targetID = response.groupId;
                ToolPage * sourcePage = d->toolBox->targetPage(sourceID);
                ToolPage * targetPage = d->toolBox->targetPage(targetID);
                bool t_rmResult = sourcePage->removeItem(d->m_movedItem);
                if(t_rmResult){
                    d->m_movedItem->setToolPage(targetPage);
                    targetPage->addItem(d->m_movedItem);
                    reconnectItem(d->m_movedItem,sourcePage,targetPage);
                    RSingleton<UserFriendContainer>::instance()->moveUser(response.oldGroupId,response.groupId,response.user.accountId);
                    updateGroupDescInfo();
                }
            }else{

            }
            break;
        }
    case G_Friend_Delete:
        {
             if(result == STATUS_SUCCESS){
                 removeContact(response.user.accountId);
                 updateGroupDescInfo();
                 emit userDeleted(CHAT_C2C,response.user.accountId);
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
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(PanelPersonPage);

    ToolItem *t_modifyItem = d->toolBox->selectedItem();
    if(t_modifyItem && d->pageOfTriggeredItem && remark != t_modifyItem->getName())
    {
        GroupingFriendRequest * t_request = new GroupingFriendRequest();
        t_request->type = G_Friend_UPDATE;
        t_request->stype = OperatePerson;
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
        RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(t_request);
    }
}

/*!
 * @brief 在更新并收到数据更新通知后，刷新联系人列表
 * @details UserFriendContainer中保存了远程分组中的最新分组和用户在线情况，需要以此作为判断标准；
 *          [1]遍历UserFriendContainer中的分组，存在几种分组情形：
 *              1.远程分组和本地分组均可找到【可设置名称】；
 *              2.远程分组存在，本地分组不存在【创建新的分组】；
 *              3.远程分组不存在，本地分组存在【从本地分组中移除，再结合本地分组中联系人存在的位置】；
 *          [2]对于每个分组中item也存在几种情形：
 *              1.远程存在，本地不存在；【直接创建】
 *              2.远程与本地均在同一分组；【设置基本的信息是否一致，对比头像，用户名，签名等】
 *              3.远程和本地不在同一分组；【从其它分组移动至当前分组】
 *              4.远程不存在，本地存在；【删除联系人，关闭对应的窗口，释放client】
 *          [3]对分组进行排序
 *          [4]对分组内的在线用户进行优先排序
 */
void PanelPersonPage::updateContactList()
{
    MQ_D(PanelPersonPage);
    const QList<RGroupData *> list = RSingleton<UserFriendContainer>::instance()->list();
    for(int groupIndex = 0;groupIndex < list.count();groupIndex++)
    {
        RGroupData * remoteGroupData = list.at(groupIndex);
        ToolPage * targetPage =  d->toolBox->targetPage(remoteGroupData->groupId);
        if(targetPage == NULL)
        {
            //1.2
            RGroupData * t_groupData = list.at(groupIndex);
            ToolPage * newPage = d->toolBox->addPage(t_groupData->groupName);
            newPage->setID(t_groupData->groupId);
            newPage->setDefault(t_groupData->isDefault);

            updateGroupDescInfo(newPage);
            newPage->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
        }
    }

    //1.1
    for(int groupIndex = 0;groupIndex < list.count();groupIndex++)
    {
        RGroupData * remoteGroupData = list.at(groupIndex);
        ToolPage * targetPage =  d->toolBox->targetPage(remoteGroupData->groupId);
        if(targetPage)
        {
            targetPage->setToolName(remoteGroupData->groupName);

            QList<SimpleUserInfo *> t_users = remoteGroupData->users;

            auto func = [](ToolItem * item,QString accountId){
                    UserClient * client = RSingleton<UserManager>::instance()->client(item);
                    if(client)
                        return client->simpleUserInfo.accountId == accountId;
                    return false;
                };

            for(int t_userIndex = 0;t_userIndex < t_users.count();t_userIndex++)
            {
                SimpleUserInfo * remoteSimpleUserInfo = t_users.at(t_userIndex);
                QString t_userId = remoteSimpleUserInfo->accountId;

                bool t_matchUserResult = targetPage->containsItem(std::bind(func,::placeholders::_1,remoteSimpleUserInfo->accountId));

                //2.1
                if(t_matchUserResult)
                {
                    UserClient * client = RSingleton<UserManager>::instance()->client(t_userId);
                    client->simpleUserInfo = *remoteSimpleUserInfo;

                    ToolItem * item = client->toolItem;
                    item->setName(remoteSimpleUserInfo->remarks);
                    item->setNickName(remoteSimpleUserInfo->nickName);
                    item->setDescInfo(remoteSimpleUserInfo->signName);

                    if(!remoteSimpleUserInfo->isSystemIcon && !QFileInfo(G_User->getFilePath(remoteSimpleUserInfo->iconId)).exists() && remoteSimpleUserInfo->iconId.size() > 0)
                     {
                         SimpleFileItemRequest * request = new SimpleFileItemRequest;
                         request->control = T_REQUEST;
                         request->itemType = FILE_ITEM_USER_DOWN;
                         request->itemKind = FILE_IMAGE;
                         request->fileId = QFileInfo(remoteSimpleUserInfo->iconId).baseName();
                         FileRecvTask::instance()->addRecvItem(request);
                     }else{
                        item->setIcon(G_User->getIcon(remoteSimpleUserInfo->isSystemIcon,remoteSimpleUserInfo->iconId));
                    }

                    item->setStatus(remoteSimpleUserInfo->status);
                }
                else{
                    UserClient * localUserClient = RSingleton<UserManager>::instance()->client(remoteSimpleUserInfo->accountId);
                    //2.2
                    if(localUserClient == NULL){
                        ToolItem * t_newItem = ceateItem(remoteSimpleUserInfo,targetPage);
                        targetPage->addItem(t_newItem);
                        //1.查找用户界面需要判断此用户是否已经被添加了
                        RSingleton<Subject>::instance()->notify(MESS_RELATION_FRIEND_ADD);
                    }
                    //2.3
                    else{
                        ToolPage * t_sourcePage = localUserClient->toolItem->toolpage();
                        bool flag = t_sourcePage->removeItem(localUserClient->toolItem);
                        if(flag){
                            targetPage->addItem(localUserClient->toolItem);
                            localUserClient->toolItem->setToolPage(targetPage);
                            reconnectItem(localUserClient->toolItem,t_sourcePage,targetPage);

                            localUserClient->toolItem->setName(remoteSimpleUserInfo->remarks);
                            localUserClient->toolItem->setNickName(remoteSimpleUserInfo->nickName);
                            localUserClient->toolItem->setDescInfo(remoteSimpleUserInfo->signName);

                            if(!remoteSimpleUserInfo->isSystemIcon && !QFileInfo(G_User->getFilePath(remoteSimpleUserInfo->iconId)).exists() && remoteSimpleUserInfo->iconId.size() > 0)
                             {
                                 SimpleFileItemRequest * request = new SimpleFileItemRequest;
                                 request->control = T_REQUEST;
                                 request->itemType = FILE_ITEM_USER_DOWN;
                                 request->itemKind = FILE_IMAGE;
                                 request->fileId = QFileInfo(remoteSimpleUserInfo->iconId).baseName();
                                 FileRecvTask::instance()->addRecvItem(request);
                             }else{
                                localUserClient->toolItem->setIcon(G_User->getIcon(remoteSimpleUserInfo->isSystemIcon,remoteSimpleUserInfo->iconId));
                            }

                            localUserClient->toolItem->setStatus(remoteSimpleUserInfo->status);
                        }
                    }
                }
            }
            updateGroupDescInfo(targetPage);
        }
    }

    QList<QString> unExistedPages;
    const QList<ToolPage *> oldPages =  d->toolBox->allPages();
    for(int pageIndex = 0; pageIndex < oldPages.count(); pageIndex++)
    {
        ToolPage * targetPage = oldPages.at(pageIndex);
        RGroupData * groupData = RSingleton<UserFriendContainer>::instance()->element(targetPage->getID());
        if(groupData == nullptr){
            unExistedPages.append(targetPage->getID());
        }
    }

    //1.3
    std::for_each(unExistedPages.begin(),unExistedPages.end(),[this](QString groupId){
        this->removeTargetGroup(groupId);
    });

    //2.4
    const QList<ToolPage *> newPages =  d->toolBox->allPages();
    for(int pageIndex = 0; pageIndex < newPages.count(); pageIndex++)
    {
        ToolPage * targetPage = newPages.at(pageIndex);
        RGroupData * groupData = RSingleton<UserFriendContainer>::instance()->element(targetPage->getID());
        if(groupData){
            QList<ToolItem *> items = targetPage->items();
            for(int itemIndex = items.count() - 1 ; itemIndex >= 0; itemIndex--)
            {
                ToolItem * t_curItem = items.at(itemIndex);
                UserClient * client = RSingleton<UserManager>::instance()->client(t_curItem);
                bool contained = RSingleton<UserFriendContainer>::instance()->containUser(client->simpleUserInfo.accountId);

                if(!contained){
                    removeContact(client->simpleUserInfo.accountId);
                }
            }
        }
    }

    updateGroupDescInfo();

    //3
    resortToolPage();

    //4
}

/*!
 * @brief 接收分组操作结果。若执行错误，则需要撤销之前的操作。
 * @details 以下操作需要同步至UserFriendContainer，确保UserFriendContainer和数据库中保持一致。
 *          1.创建分组：
 *          2.重命名分组：
 *          3.删除分组：
 *          4.排序分组：
 * @param[in] response 分组操作结果信息
 */
void PanelPersonPage::recvFriendPageOperate(MsgOperateResponse status,GroupingResponse response)
{
    MQ_D(PanelPersonPage);

    if(response.uuid != G_User->BaseInfo().uuid)
        return;

    if(status == STATUS_SUCCESS){
        switch(response.type){
            case GROUPING_CREATE:{
                    RSingleton<UserFriendContainer>::instance()->addGroup(response.groupId,response.groupIndex);
                }
                break;
            case GROUPING_RENAME:{
                    RSingleton<UserFriendContainer>::instance()->renameGroup(response.groupId);
                }
                break;
            case GROUPING_DELETE:{
                    removeTargetGroup(d->m_deleteID);
                }
                break;
            case GROUPING_SORT:{
                    d->toolBox->sortPage(response.groupId,response.groupIndex);
                    RSingleton<UserFriendContainer>::instance()->sortGroup(response.groupId,response.groupIndex);
                }
                break;
            default:break;
        }
        updateGroupDescInfo();
    }else if(status == STATUS_FAILE){
        switch(response.type){
            case GROUPING_CREATE:{
                    RSingleton<UserFriendContainer>::instance()->removeTmpGroup(response.groupId);
                    removeTargetGroup(response.groupId);
                }
                break;
            case GROUPING_RENAME:{
                    QString originGroupName = RSingleton<UserFriendContainer>::instance()->revertRenameGroup(response.groupId);
                    ToolPage * selectedPage = d->toolBox->targetPage(response.groupId);
                    if(selectedPage)
                        selectedPage->setToolName(originGroupName);
                }
                break;
            case GROUPING_DELETE:{
                    d->m_deleteID = QString();
                }
                break;
            case GROUPING_SORT:{
                    d->toolBox->revertSortPage(response.groupId);
                }
                break;
            default:break;
        }
        RMessageBox::warning(this,QObject::tr("warning"),tr("Opearate failed!"),RMessageBox::Yes);
    }
}

void PanelPersonPage::loadHistoryMsg()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;

    HistoryMessRequest * request = new HistoryMessRequest();
    request->accountId = G_User->BaseInfo().accountId;
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
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
    connect(item,SIGNAL(itemDoubleClick(ToolItem*)),this,SLOT(showChatWindow(ToolItem*)));
    connect(item,SIGNAL(itemMouseHover(bool,ToolItem*)),MainDialog::instance(),SLOT(showHoverItem(bool,ToolItem*)));
    connect(item,SIGNAL(updateGroupActions()),page,SLOT(updateGroupActions()));

    item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM));
    item->setName(userInfo->remarks);
    item->setNickName(userInfo->nickName);
    item->setDescInfo(userInfo->signName);

    if(!userInfo->isSystemIcon && !QFile(G_User->getFilePath(userInfo->iconId)).exists() && userInfo->iconId.size() > 0)
     {
         SimpleFileItemRequest * request = new SimpleFileItemRequest;
         request->control = T_REQUEST;
         request->itemType = FILE_ITEM_USER_DOWN;
         request->itemKind = FILE_IMAGE;
         request->fileId = QFileInfo(userInfo->iconId).baseName();
         FileRecvTask::instance()->addRecvItem(request);
     }else{
        item->setIcon(G_User->getIcon(userInfo->isSystemIcon,userInfo->iconId));
    }

    item->setStatus(userInfo->status);

    emit userStateChanged(userInfo->status,userInfo->accountId);

    UserClient * client = RSingleton<UserManager>::instance()->addClient(userInfo->accountId);
    client->simpleUserInfo = *userInfo;
    client->toolItem = item;

#ifdef __LOCAL_CONTACT__
    if(G_ParaSettings){
        QVector<ParameterSettings::OuterNetConfig>::iterator iter = std::find_if(G_ParaSettings->outerNetConfig.begin(),G_ParaSettings->outerNetConfig.end(),[&userInfo](const ParameterSettings::OuterNetConfig & outerConfig){
            return outerConfig.nodeId == userInfo->accountId;
        });
        if(iter != G_ParaSettings->outerNetConfig.end()){
            client->netConfig = *iter;
        }
    }
#endif
    return item;
}

/*!
 * @brief LineEdit中命名完成后将内容设置为page的name
 * @attention 创建临时的数据分组信息添加至UserFriendContainer的tmpCreateOrRenameMap
 */
void PanelPersonPage::renameEditFinished()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(PanelPersonPage);
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
        request->gtype = OperatePerson;
        request->groupName = d->tmpNameEdit->text();
        request->groupId = d->toolBox->selectedPage()->getID();

        RGroupData * tmpData = new RGroupData;
        tmpData->groupId = request->groupId;
        tmpData->groupName = request->groupName;
        tmpData->isDefault = false;
        RSingleton<UserFriendContainer>::instance()->addTmpGroup(request->groupId,tmpData);

        RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
        d->groupIsCreate = true;
    }
    d->tmpNameEdit->clear();
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
    if(movePersonTo){
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
}

/*!
 * @brief 移动至各分组的Action响应
 * @details 只处理移动的请求，待服务器移动成功后，再真实的移动
 * @param
 * @return 无
 */
void PanelPersonPage::movePersonTo()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
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
        request->stype = OperatePerson;
        request->groupId = t_targetPage->getID();
        request->oldGroupId = t_sourcePage->getID();

        UserClient * client = RSingleton<UserManager>::instance()->client(d->m_movedItem);
        if(client)
        {
            request->user = client->simpleUserInfo;
        }
        RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
    }
}

/*!
 * @brief 响应自己/好友状态更新
 * @details 【1】好友更新：根据接收到的信息，更新当前好友的列表信息，同时更新当前页面中的控件的显示。
 *          【2】自己更新：若为离线状态，则需要将当前联系人列表中分组在线数量进行修改；
 * @param[in] result 操作结果
 * @param[in] response 好友状态信息
 * @return 无
 */
void PanelPersonPage::recvUserStateChanged(MsgOperateResponse result, UserStateResponse response)
{
    if(result == STATUS_SUCCESS)
    {
        if(response.accountId != G_User->BaseInfo().accountId){
            UserClient * client = RSingleton<UserManager>::instance()->client(response.accountId);
            if(client)
            {
                client->toolItem->setStatus(response.onStatus);
                client->simpleUserInfo.status = response.onStatus;
                updateGroupDescInfo();
                if(response.onStatus != STATUS_OFFLINE && response.onStatus != STATUS_HIDE)
                {
                    if(G_User->systemSettings()->soundAvailable)
                        RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaOnline);
                }
                emit userStateChanged(response.onStatus,response.accountId);
            }
        }else{
            //执行断网操作
            if(response.onStatus == STATUS_OFFLINE){
                RSingleton<Subject>::instance()->notify(MESS_USER_OFF_LINE);
            }
        }
    }
}

void PanelPersonPage::createAction()
{
    MQ_D(PanelPersonPage);
#ifndef __LOCAL_CONTACT__
    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLBOX);

    QAction * freshAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_REFRESH,this,SLOT(refreshList()));
    freshAction->setText(tr("Refresh list"));

    QAction * addGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_ADDGROUP,this,SLOT(respGroupCreate()));
    addGroupAction->setText(tr("Add group"));

    menu->addAction(freshAction);
    menu->addSeparator();
    menu->addAction(addGroupAction);

    d->toolBox->setContextMenu(menu);

    //创建分组的右键菜单
    QMenu * groupMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLGROUP);

    QAction * renameAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_RENAME,this,SLOT(respGroupRename()));
    renameAction->setText(tr("Rename"));

    QAction * deleteGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_DELGROUP,this,SLOT(respGroupDeleted()));
    deleteGroupAction->setText(tr("Delete group"));

    groupMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_REFRESH));
    groupMenu->addSeparator();
    groupMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_ADDGROUP));
    groupMenu->addAction(renameAction);
    groupMenu->addAction(deleteGroupAction);
#endif
    //创建联系人的右键菜单
    QMenu * personMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLITEM);

    QAction * sendMessAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_SENDMESSAGE,this,SLOT(sendInstantMessage()));
    sendMessAction->setText(tr("Send Instant Message"));
#ifndef __LOCAL_CONTACT__
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
#endif
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_SENDMESSAGE));
#ifndef __LOCAL_CONTACT__
    personMenu->addSeparator();
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_VIEWDETAIL));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MODIFYCOMMENTS));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_DELPERSON));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MOVEPERSON));
    personMenu->addSeparator();
#endif
}
