#include "userclient.h"

#include "user/user.h"
#include "global.h"
#include "rsingleton.h"
#include "Widgets/notifywindow.h"
#include "Util/imagemanager.h"
#include "json/jsonresolver.h"
#include "util/rutil.h"
#include "sql/sqlprocess.h"
#include "media/mediaplayer.h"
#include "messdiapatch.h"
#include "thread/filerecvtask.h"
#include "file/filedesc.h"
#include "Widgets/toolbox/toolitem.h"
#include "../protocol/datastruct.h"
#include "thread/chatmsgprocess.h"
#include "../Widgets/chatpersonwidget.h"

#include <QDebug>

UserClient::UserClient():toolItem(nullptr),chatPersonWidget(nullptr)
{
}

UserClient::~UserClient()
{
    if(chatPersonWidget && chatPersonWidget->isVisible())
    {
        chatPersonWidget->close();
        delete chatPersonWidget;
        chatPersonWidget = NULL;
    }
}

//TODO 考虑若未打开窗口，消息如何存储？？
/*!
 * @brief 接收发送的聊天消息
 * @details
 *    1.存储消息至数据库；
 *    2.将信息添加至历史会话列表;
 *    3.  若聊天对象的窗口未创建，则使用系统通知，进行提示；
 *        若聊天对象的窗口已创建，但不可见，则将信息保存并将窗口设置可见
 * @param[in] response 消息体内容
 * @return 是否插入成功
 */
void UserClient::procRecvContent(TextRequest & response)
{
    if(response.msgCommand == MSG_TEXT_TEXT || response.msgCommand == MSG_TEXT_SHAKE){
        //【1】存储消息至数据库
        ChatInfoUnit t_unit;
        t_unit.contentType = MSG_TEXT_TEXT;
        t_unit.accountId = simpleUserInfo.accountId;
        t_unit.nickName = simpleUserInfo.nickName;
        t_unit.dtime = RUtil::currentMSecsSinceEpoch();
        t_unit.contents = response.sendData;
        RSingleton<ChatMsgProcess>::instance()->appendC2CStoreTask(t_unit);

        //TODO 20180423【2】将信息添加至历史会话列表
        HistoryChatRecord record;
        record.accountId = simpleUserInfo.accountId;
        record.nickName = simpleUserInfo.nickName;
        record.dtime = RUtil::currentMSecsSinceEpoch();
        record.lastRecord = RUtil::getTimeStamp();
        record.systemIon = simpleUserInfo.isSystemIcon;
        record.iconId = simpleUserInfo.iconId;
        record.type = CHAT_C2C;
        MessDiapatch::instance()->onAddHistoryItem(record);

        if(chatPersonWidget && chatPersonWidget->isVisible())
        {
            if(response.msgCommand == MSG_TEXT_TEXT)
            {
                chatPersonWidget->recvChatMsg(response);
            }
            else if(response.msgCommand == MSG_TEXT_SHAKE)
            {
                if(G_User->systemSettings()->windowShaking)
                    chatPersonWidget->shakeWindow();
            }
        }
        else
        {
            if(response.msgCommand == MSG_TEXT_SHAKE)
            {
                if(!chatPersonWidget)
                {
                    chatPersonWidget = new ChatPersonWidget();
                    chatPersonWidget->setUserInfo(simpleUserInfo);
                    chatPersonWidget->initChatRecord();
                }
                chatPersonWidget->show();

            }
            else if(response.msgCommand == MSG_TEXT_TEXT)
            {
                //TODO 未将文本消息设置到提示框中，待对加密、压缩等信息处理
                NotifyInfo  info;
                info.identityId = RUtil::UUID();
                info.msgCommand = response.msgCommand;
                info.accountId = response.otherSideId;
                info.nickName = simpleUserInfo.nickName;
                info.type = NotifyUser;
                info.stype = response.type;
                info.isSystemIcon = simpleUserInfo.isSystemIcon;
                info.iconId = simpleUserInfo.iconId;

                RSingleton<NotifyWindow>::instance()->addNotifyInfo(info);
                RSingleton<NotifyWindow>::instance()->showMe();
            }
        }

        if(G_User->systemSettings()->soundAvailable)
        {
            if(response.msgCommand == MSG_TEXT_TEXT)
                RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaMsg);
            else if(response.msgCommand == MSG_TEXT_SHAKE)
                RSingleton<MediaPlayer>::instance()->play(MediaPlayer::MediaShake);
        }
    }
    else if(response.msgCommand == MSG_TEXT_FILE || response.msgCommand == MSG_TEXT_AUDIO || response.msgCommand == MSG_TEXT_IMAGE){
        //【聊天文件发送:4/5】拿着fileId向文件服务器下载对应文件
        SimpleFileItemRequest * request = new SimpleFileItemRequest;
        request->control = T_REQUEST;
        request->itemType = FILE_ITEM_CHAT_DOWN;
        if(response.msgCommand == MSG_TEXT_FILE)
        {
            request->itemKind = FILE_FILE;
        }
        else if(response.msgCommand == MSG_TEXT_AUDIO)
        {
            request->itemKind = FILE_AUDIO;
        }else if(response.msgCommand == MSG_TEXT_IMAGE)
        {
            request->itemKind = FILE_IMAGE;
        }
        request->fileId = response.sendData;
        FileRecvTask::instance()->addRecvItem(request);
    }
}

/*!
 * @brief 处理服务器端返回接收消息的确认消息
 * @param[in] reply 确认消息
 */
void UserClient::procRecvServerTextReply(TextReply & reply)
{
    switch(reply.applyType)
    {
        case APPLY_SYSTEM:
             {

             }
             break;
        default:
            break;
    }
}

/*!
 * @brief 处理下载完成用户文件
 * @param[in] fileDesc 文件标识
 */
void UserClient::procDownOverFile(FileDesc *fileDesc)
{
    //【聊天文件发送:5/5】下载完指定fileId的文件，分发至对应的窗口
    FileItemKind itemKind = static_cast<FileItemKind>(fileDesc->itemKind);
    if(chatPersonWidget && chatPersonWidget->isVisible()){
        if(itemKind == FILE_FILE){

        }else if(itemKind == FILE_AUDIO){
            //TODO LYS-20180608 显示接收到的语音消息
//            chatPersonWidget->appendVoiceMsg(RECV,fileDesc->fileName);
        }else if(itemKind == FILE_IMAGE){

        }
    }else{

    }
}

/*!
 * @brief 处理用户头像下载
 * @param[in] fileDesc 文件标识
 */
void UserClient::procDownItemIcon(FileDesc *fileDesc)
{
    toolItem->setIcon(G_User->getIcon(simpleUserInfo.isSystemIcon,simpleUserInfo.iconId));
}

UserManager::UserManager()
{

}

/*!
 * @brief 添加联系人
 * @param[in] accountId:QString 联系人id
 * @return 联系人:UserClient *
 */
UserClient *UserManager::addClient(QString accountId)
{
    std::lock_guard<std::mutex> lg(clientMutex);
    if(clients.contains(accountId))
        return clients.value(accountId);

    UserClient * client = new UserClient;
    clients.insert(accountId,client);

    return client;
}

/*!
 * @brief 移除联系人
 * @param[in] accountId:QString 联系人id
 * @return 移除联系人结果:bool
 */
bool UserManager::removeClient(QString accountId)
{
    std::lock_guard<std::mutex> lg(clientMutex);
    bool t_removeResult = false;
    if(clients.contains(accountId))
    {
        delete clients.value(accountId);
        int t_result = clients.remove(accountId);
        if(t_result != 0){
            t_removeResult = true;
        }
    }
    return t_removeResult;
}

UserClient *UserManager::client(ToolItem *item)
{
    std::unique_lock<std::mutex> ul(clientMutex);
    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->toolItem == item)
            return iter.value();
        iter++;
    }

    return NULL;
}

UserClient *UserManager::client(QString accountId)
{
    std::unique_lock<std::mutex> ul(clientMutex);
    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
        if(iter.value()->simpleUserInfo.accountId == accountId)
            return iter.value();
        iter++;
    }

    return NULL;
}

void UserManager::for_each(std::function<void(UserClient*)> func)
{
    std::lock_guard<std::mutex> lg(clientMutex);
    auto beg = clients.begin();
    while(beg != clients.end()){
        func(beg.value());
        beg++;
    }
}

//TODO 关闭窗口出现问题
void UserManager::closeAllClientWindow()
{
    std::lock_guard<std::mutex> lg(clientMutex);
    QHash<QString,UserClient*>::iterator iter = clients.begin();
    while(iter != clients.end())
    {
//       if(iter.value()->chatWidget)
//       {
////           delete iter.value()->chatWidget;
//       }

       if(iter.value()->chatPersonWidget)
       {
//           delete iter.value()->chatPersonWidget;
           iter.value()->chatPersonWidget->close();
       }
       iter++;
    }
}

int UserManager::size()
{
    std::lock_guard<std::mutex> lg(clientMutex);
    return clients.size();
}
