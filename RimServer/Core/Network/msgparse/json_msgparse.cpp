#include "json_msgparse.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QSharedPointer>
#include <memory>
#include <QDebug>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "protocol/protocoldata.h"
#include "jsonkey.h"
#include "global.h"
#include "../msgprocess/dataprocess.h"

using namespace ProtocolType;

Json_MsgParse::Json_MsgParse()
{

}

/*!
 * @brief 分发数据类型
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void Json_MsgParse::processData(Database *db,const SocketInData &data)
{
    if(RGlobal::G_SERVICE_TYPE == SERVICE_TEXT)
    {
        QJsonParseError errorInfo;
        QJsonDocument document = QJsonDocument::fromJson(data.data,&errorInfo);
        if(errorInfo.error == QJsonParseError::NoError)
        {
            QJsonObject obj = document.object();
            switch(obj.value(JsonKey::key(JsonKey::Type)).toInt())
            {
                case MSG_CONTROL:
                    parseControlData(db,data.sockId,obj);
                    break;
                case MSG_TEXT:
                    parseTextData(db,data.sockId,obj);
                    break;
                default:
                      break;
            }
        }
        else
        {
            RLOG_ERROR("recv a error data,skip!");
        }
    }
    else if(RGlobal::G_SERVICE_TYPE == SERVICE_FILE)
    {
        RBuffer buffer(data.data);
        int msgType,msgCommand;
        if(!buffer.read(msgType) || !buffer.read(msgCommand))
            return;
        if((MsgType)msgType == MSG_FILE)
        {
            parseFileData(db,data.sockId,buffer);
        }
    }
}

/*!
     * @brief 解析控制报文
     * @details 处理用户登陆注册、分组操作、群组操作等
     * @param[in] db 线程拥有的数据库
     * @param[in] socketId 访问的socketId
     * @param[in] obj 解析后json请求数据
     * @return 无
     */
void Json_MsgParse::parseControlData(Database * db,int socketId,QJsonObject &obj)
{
    switch(obj.value(JsonKey::key(JsonKey::Command)).toInt())
    {
        case MSG_USER_REGISTER:
              onProcessUserRegist(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_USER_LOGIN:
              onProcessUserLogin(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_USER_UPDATE_INFO:
              onProcessUpdateUserInfo(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_USER_STATE:
              onProcessUserStateChanged(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_RELATION_SEARCH:
              onProcessSearchFriend(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_REALTION_ADD:
              onProcessAddFriend(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_RELATION_OPERATE:
              onProcessRelationOperate(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_RELATION_LIST:
              onProcessFriendListOperate(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_GROUPING_OPERATE:
              onProcessGroupingOperate(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_RELATION_GROUPING_FRIEND:
              onProcessGroupingFriend(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_GROUP_LIST:
              onProcessGroupList(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_GROUP_CREATE:
              onProcessRegistGroup(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_GROUP_COMMAND:
              onProcessGroupCommand(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
        case MSG_USER_HISTORY_MSG:
              onProcessHistoryMsg(db,socketId,obj.value(JsonKey::key(JsonKey::Data)).toObject());
              break;
         default:
             break;
    }
}

/*!
 * @brief 解析消息文本
 * @param[in] db 线程拥有的数据库
 * @param[in] socketId 访问的socketId
 * @param[in] obj 解析后json请求数据
 * @return 无
 */
void Json_MsgParse::parseTextData(Database * db,int socketId,QJsonObject &obj)
{
    QJsonObject dataObj = obj.value(JsonKey::key(JsonKey::Data)).toObject();
    if(!dataObj.isEmpty())
    {
        QSharedPointer<TextRequest> request(new TextRequest);
        request->msgCommand = (MsgCommand)obj.value(JsonKey::key(JsonKey::Command)).toInt();

        request->accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
        request->textId = dataObj.value(JsonKey::key(JsonKey::TextId)).toString();
        request->otherSideId = dataObj.value(JsonKey::key(JsonKey::OtherSideId)).toString();
        request->type = (OperateType)dataObj.value(JsonKey::key(JsonKey::OperateType)).toInt();
        request->timeStamp = dataObj.value(JsonKey::key(JsonKey::Time)).toVariant().toULongLong();
        request->isEncryption = dataObj.value(JsonKey::key(JsonKey::Encryption)).toBool();
        request->isCompress = dataObj.value(JsonKey::key(JsonKey::Compress)).toBool();
        request->textType = (TextType)dataObj.value(JsonKey::key(JsonKey::Type)).toInt();

        request->sendData = dataObj.value(JsonKey::key(JsonKey::Data)).toString();

        RSingleton<DataProcess>::instance()->processText(db,socketId,request);
    }
}

/*!
 * @brief 解析文件传输过程中各种消息
 * @param[in] db 线程拥有的数据库
 * @param[in] socketId 访问的socketId
 * @param[in] obj 解析后json请求数据
 * @return 无
 */
void Json_MsgParse::parseFileData(Database *db, int socketId, RBuffer &buffer)
{
    if(buffer.seek(0) && buffer.skipInt())
    {
        int commandType;
        if(buffer.read(commandType))
        {
            switch((MsgCommand)commandType)
            {
                case MSG_FILE_REQUEST:
                    onProcessFileRequest(db,socketId,buffer);
                    break;
                case MSG_FILE_CONTROL:
                    onProcessFileControl(db,socketId,buffer);
                    break;
                case MSG_FILE_DATA:
                    onProcessFileData(db,socketId,buffer);
                    break;
            }
        }
    }
}

void Json_MsgParse::onProcessUserRegist(Database * db,int socketId,QJsonObject &obj)
{
    std::shared_ptr<RegistRequest> request(new RegistRequest);
    request->nickName = obj.value(JsonKey::key(JsonKey::NickName)).toString();
    request->password = obj.value(JsonKey::key(JsonKey::Pass)).toString();

    RSingleton<DataProcess>::instance()->processUserRegist(db,socketId,request);
}

void Json_MsgParse::onProcessUserLogin(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<LoginRequest> request(new LoginRequest);
    request->loginType = (LoginType)obj.value(JsonKey::key(JsonKey::LoginType)).toInt();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->password = obj.value(JsonKey::key(JsonKey::Pass)).toString();
    request->status = (OnlineStatus)obj.value(JsonKey::key(JsonKey::Status)).toInt();

    RSingleton<DataProcess>::instance()->processUserLogin(db,socketId,request);
}

void Json_MsgParse::onProcessUpdateUserInfo(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<UpdateBaseInfoRequest> request (new UpdateBaseInfoRequest);
    request->baseInfo.accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->baseInfo.nickName = obj.value(JsonKey::key(JsonKey::NickName)).toString();
    request->baseInfo.signName = obj.value(JsonKey::key(JsonKey::SignName)).toString();
    request->baseInfo.sexual = (Sexual)obj.value(JsonKey::key(JsonKey::Sexual)).toInt();
    request->baseInfo.birthday = obj.value(JsonKey::key(JsonKey::Birth)).toString();
    request->baseInfo.address = obj.value(JsonKey::key(JsonKey::Address)).toString();
    request->baseInfo.email = obj.value(JsonKey::key(JsonKey::Email)).toString();
    request->baseInfo.phoneNumber = obj.value(JsonKey::key(JsonKey::Phone)).toString();
    request->baseInfo.remark = obj.value(JsonKey::key(JsonKey::Remark)).toString();
    request->baseInfo.isSystemIcon = obj.value(JsonKey::key(JsonKey::SystemIcon)).toBool();
    request->baseInfo.iconId = obj.value(JsonKey::key(JsonKey::IconId)).toString();

    RSingleton<DataProcess>::instance()->processUpdateUserInfo(db,socketId,request);
}

void Json_MsgParse::onProcessUserStateChanged(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<UserStateRequest> request (new UserStateRequest());
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->onStatus = (OnlineStatus)obj.value(JsonKey::key(JsonKey::Status)).toInt();

    RSingleton<DataProcess>::instance()->processUserStateChanged(db,socketId,request);
}

void Json_MsgParse::onProcessHistoryMsg(Database *db, int socketId, QJsonObject &obj)
{
    QSharedPointer<HistoryMessRequest> request (new HistoryMessRequest());
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();

    RSingleton<DataProcess>::instance()->processHistoryMsg(db,socketId,request);
}

void Json_MsgParse::onProcessSearchFriend(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<SearchFriendRequest> request (new SearchFriendRequest());
    request->stype = (OperateType)obj.value(JsonKey::key(JsonKey::OperateType)).toInt();
    request->accountOrNickName = obj.value(JsonKey::key(JsonKey::SearchContent)).toString();

    RSingleton<DataProcess>::instance()->processSearchFriend(db,socketId,request);
}

void Json_MsgParse::onProcessRelationOperate(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<OperateFriendRequest> request (new OperateFriendRequest);
    request->type = (OperateFriend)obj.value(JsonKey::key(JsonKey::Type)).toInt();
    request->result = (ResponseFriendApply)obj.value(JsonKey::key(JsonKey::Result)).toInt();
    request->stype = (OperateType)obj.value(JsonKey::key(JsonKey::OperateType)).toInt();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->operateId = obj.value(JsonKey::key(JsonKey::OperateId)).toString();
    request->chatId = obj.value(JsonKey::key(JsonKey::ChatId)).toString();
    request->chatName = obj.value(JsonKey::key(JsonKey::ChatName)).toString();

    RSingleton<DataProcess>::instance()->processRelationOperate(db,socketId,request);
}

void Json_MsgParse::onProcessAddFriend(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<AddFriendRequest> request (new AddFriendRequest);
    request->stype = (OperateType)obj.value(JsonKey::key(JsonKey::AddType)).toInt();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->operateId = obj.value(JsonKey::key(JsonKey::OperateId)).toString();

    RSingleton<DataProcess>::instance()->processAddFriend(db,socketId,request);
}

void Json_MsgParse::onProcessFriendListOperate(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<FriendListRequest> request (new FriendListRequest);
    request->type = static_cast<OperateListTimeType>(obj.value(JsonKey::key(JsonKey::Type)).toInt());
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();

    RSingleton<DataProcess>::instance()->processFriendList(db,socketId,request);
}

void Json_MsgParse::onProcessGroupingFriend(Database *db, int socketId, QJsonObject &dataObj)
{
    QSharedPointer<GroupingFriendRequest> request (new GroupingFriendRequest);
    request->type = (OperateGroupingFriend) dataObj.value(JsonKey::key(JsonKey::Type)).toInt();
    request->groupId = dataObj.value(JsonKey::key(JsonKey::GroupId)).toString();
    request->oldGroupId = dataObj.value(JsonKey::key(JsonKey::OldGroupId)).toString();
    request->stype = (OperateType)dataObj.value(JsonKey::key(JsonKey::OperateType)).toInt();

    QJsonObject simpleObj = dataObj.value(JsonKey::key(JsonKey::Users)).toObject();
    request->user.accountId = simpleObj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->user.nickName = simpleObj.value(JsonKey::key(JsonKey::NickName)).toString();
    request->user.signName = simpleObj.value(JsonKey::key(JsonKey::SignName)).toString();
    request->user.isSystemIcon = simpleObj.value(JsonKey::key(JsonKey::SystemIcon)).toBool();
    request->user.iconId = simpleObj.value(JsonKey::key(JsonKey::IconId)).toString();
    request->user.remarks = simpleObj.value(JsonKey::key(JsonKey::Remark)).toString();
    request->user.status = (OnlineStatus)simpleObj.value(JsonKey::key(JsonKey::Status)).toInt();

    RSingleton<DataProcess>::instance()->processGroupingFriend(db,socketId,request);
}

void Json_MsgParse::onProcessGroupingOperate(Database * db,int socketId,QJsonObject &obj)
{
    QSharedPointer<GroupingRequest> request (new GroupingRequest);
    request->uuid = obj.value(JsonKey::key(JsonKey::Uuid)).toString();
    request->groupId = obj.value(JsonKey::key(JsonKey::GroupId)).toString();
    request->gtype = (OperateType)obj.value(JsonKey::key(JsonKey::OperateType)).toInt();
    request->type = (OperateGrouping)obj.value(JsonKey::key(JsonKey::Type)).toInt();
    request->groupName = obj.value(JsonKey::key(JsonKey::GroupName)).toString();
    request->groupIndex = obj.value(JsonKey::key(JsonKey::Index)).toInt();

    RSingleton<DataProcess>::instance()->processGroupingOperate(db,socketId,request);
}

void Json_MsgParse::onProcessGroupList(Database *db, int socketId, QJsonObject &obj)
{
    QSharedPointer<ChatGroupListRequest> request (new ChatGroupListRequest);
    request->uuid = obj.value(JsonKey::key(JsonKey::Uuid)).toString();

    RSingleton<DataProcess>::instance()->processGroupList(db,socketId,request);
}

void Json_MsgParse::onProcessRegistGroup(Database *db, int socketId, QJsonObject &obj)
{
    QSharedPointer<RegistGroupRequest> request (new RegistGroupRequest);
    request->groupName = obj.value(JsonKey::key(JsonKey::GroupName)).toString();
    request->groupDesc = obj.value(JsonKey::key(JsonKey::Desc)).toString();
    request->groupLabel = obj.value(JsonKey::key(JsonKey::Label)).toString();
    request->searchVisible = obj.value(JsonKey::key(JsonKey::SearchVisible)).toBool();
    request->validateAble = obj.value(JsonKey::key(JsonKey::ValidateAble)).toBool();
    request->validateQuestion = obj.value(JsonKey::key(JsonKey::Question)).toString();
    request->validateAnaswer = obj.value(JsonKey::key(JsonKey::Answer)).toString();
    request->userId = obj.value(JsonKey::key(JsonKey::Uuid)).toString();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();

    RSingleton<DataProcess>::instance()->processRegistGroup(db,socketId,request);
}

void Json_MsgParse::onProcessGroupCommand(Database *db, int socketId, QJsonObject &obj)
{
    QSharedPointer<GroupingCommandRequest> request (new GroupingCommandRequest);

    request->type = static_cast<OperateGroupingChat>(obj.value(JsonKey::key(JsonKey::Type)).toInt());
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->groupId = obj.value(JsonKey::key(JsonKey::GroupId)).toString();
    request->chatRoomId = obj.value(JsonKey::key(JsonKey::ChatRoomId)).toString();
    request->operateId = obj.value(JsonKey::key(JsonKey::OperateId)).toString();

    RSingleton<DataProcess>::instance()->processGroupCommand(db,socketId,request);
}

void Json_MsgParse::onProcessFileRequest(Database *db, int socketId, RBuffer &obj)
{
    QSharedPointer<FileItemRequest> request (new FileItemRequest());

    int control,itemType,itemKind;
    if(!obj.read(control))
        return;
    request->control = (FileTransferControl)control;

    if(!obj.read(itemType))
        return;
    request->itemType = (FileItemType)itemType;

    if(!obj.read(itemKind))
        return;
    request->itemKind = (FileItemKind)itemKind;

    if(!obj.read(request->fileName))
        return;

    if(!obj.read(request->size))
        return;

    if(!obj.read(request->fileId))
        return;

    if(!obj.read(request->md5))
        return;

    if(!obj.read(request->accountId))
        return;

    if(!obj.read(request->otherId))
        return;

    RSingleton<DataProcess>::instance()->processFileRequest(db,socketId,request);
}

/*!
 * @brief 响应用于文件传输控制命令
 * @param[in] db 数据库链接
 * @param[in] socketId 网络标识
 * @param[in] obj 数据缓冲区
 * @return 无
 */
void Json_MsgParse::onProcessFileControl(Database *db, int socketId, RBuffer &obj)
{
    QSharedPointer<SimpleFileItemRequest> request (new SimpleFileItemRequest());
    int controlType = 0;
    if(!obj.read(controlType))
        return;
    request->control = static_cast<FileTransferControl>(controlType);

    int itemType = 0;
    if(!obj.read(itemType))
        return;
    request->itemType = static_cast<FileItemType>(itemType);

    int itemKind = 0;
    if(!obj.read(itemKind))
        return;
    request->itemKind = static_cast<FileItemKind>(itemKind);

    if(!obj.read(request->md5))
        return;

    if(!obj.read(request->fileId))
        return;

    RSingleton<DataProcess>::instance()->processFileControl(db,socketId,request);
}

void Json_MsgParse::onProcessFileData(Database *db, int socketId, RBuffer &obj)
{
    QSharedPointer<FileDataRequest> request (new FileDataRequest());
    request->control =  T_DATA;

    if(!obj.read(request->fileId))
        return;
    if(!obj.read(request->index))
        return;

    const char * data = NULL;
    size_t dataLen = 0;
    if(!obj.read(&data,BUFFER_DEFAULT_SIZE,dataLen))
        return;
    request->array.append(data,dataLen);

    RSingleton<DataProcess>::instance()->processFileData(db,socketId,request);
}
