#include "dataparse.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QScopedPointer>
#include <QDebug>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "dataprocess.h"
#include "protocoldata.h"
#include "jsonkey.h"
#include "global.h"

using namespace ProtocolType;

DataParse::DataParse(QObject *parent) : QObject(parent)
{

}

/*!
 * @brief 分发数据类型
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void DataParse::processData(Database *db,const SocketInData &data)
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
void DataParse::parseControlData(Database * db,int socketId,QJsonObject &obj)
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
void DataParse::parseTextData(Database * db,int socketId,QJsonObject &obj)
{
    QJsonObject dataObj = obj.value(JsonKey::key(JsonKey::Data)).toObject();
    if(!dataObj.isEmpty())
    {
        TextRequest * request = new TextRequest;
        request->msgCommand = (MsgCommand)obj.value(JsonKey::key(JsonKey::Command)).toInt();

        request->accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
        request->textId = dataObj.value(JsonKey::key(JsonKey::TextId)).toString();
        request->otherSideId = dataObj.value(JsonKey::key(JsonKey::OtherSideId)).toString();
        request->type = (SearchType)dataObj.value(JsonKey::key(JsonKey::SearchType)).toInt();
        request->timeStamp = (SearchType)dataObj.value(JsonKey::key(JsonKey::Time)).toInt();
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
void DataParse::parseFileData(Database *db, int socketId, RBuffer &buffer)
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

void DataParse::onProcessUserRegist(Database * db,int socketId,QJsonObject &obj)
{
    RegistRequest * request = new RegistRequest;
    request->nickName = obj.value(JsonKey::key(JsonKey::NickName)).toString();
    request->password = obj.value(JsonKey::key(JsonKey::Pass)).toString();

    RSingleton<DataProcess>::instance()->processUserRegist(db,socketId,request);
}

void DataParse::onProcessUserLogin(Database * db,int socketId,QJsonObject &obj)
{
    LoginRequest * request = new LoginRequest;
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->password = obj.value(JsonKey::key(JsonKey::Pass)).toString();
    request->status = (OnlineStatus)obj.value(JsonKey::key(JsonKey::Status)).toInt();

    RSingleton<DataProcess>::instance()->processUserLogin(db,socketId,request);
}

void DataParse::onProcessUpdateUserInfo(Database * db,int socketId,QJsonObject &obj)
{
    UpdateBaseInfoRequest * request = new UpdateBaseInfoRequest;
    request->baseInfo.accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->baseInfo.nickName = obj.value(JsonKey::key(JsonKey::NickName)).toString();
    request->baseInfo.signName = obj.value(JsonKey::key(JsonKey::SignName)).toString();
    request->baseInfo.sexual = (Sexual)obj.value(JsonKey::key(JsonKey::Sexual)).toInt();
    request->baseInfo.birthday = obj.value(JsonKey::key(JsonKey::Birth)).toString();
    request->baseInfo.address = obj.value(JsonKey::key(JsonKey::Address)).toString();
    request->baseInfo.email = obj.value(JsonKey::key(JsonKey::Email)).toString();
    request->baseInfo.phoneNumber = obj.value(JsonKey::key(JsonKey::Phone)).toString();
    request->baseInfo.remark = obj.value(JsonKey::key(JsonKey::Remark)).toString();
    request->baseInfo.face = obj.value(JsonKey::key(JsonKey::Face)).toInt();
    request->baseInfo.customImgId = obj.value(JsonKey::key(JsonKey::FaceId)).toString();

    RSingleton<DataProcess>::instance()->processUpdateUserInfo(db,socketId,request);
}

void DataParse::onProcessUserStateChanged(Database * db,int socketId,QJsonObject &obj)
{
    UserStateRequest * request = new UserStateRequest();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->onStatus = (OnlineStatus)obj.value(JsonKey::key(JsonKey::Status)).toInt();

    RSingleton<DataProcess>::instance()->processUserStateChanged(db,socketId,request);
}

void DataParse::onProcessSearchFriend(Database * db,int socketId,QJsonObject &obj)
{
    SearchFriendRequest * request = new SearchFriendRequest;
    request->stype = (SearchType)obj.value(JsonKey::key(JsonKey::SearchType)).toInt();
    request->accountOrNickName = obj.value(JsonKey::key(JsonKey::SearchContent)).toString();

    RSingleton<DataProcess>::instance()->processSearchFriend(db,socketId,request);
}

void DataParse::onProcessRelationOperate(Database * db,int socketId,QJsonObject &obj)
{
    OperateFriendRequest * request = new OperateFriendRequest;
    request->type = (OperateFriend)obj.value(JsonKey::key(JsonKey::Type)).toInt();
    request->result = (ResponseFriendApply)obj.value(JsonKey::key(JsonKey::Result)).toInt();
    request->stype = (SearchType)obj.value(JsonKey::key(JsonKey::SearchType)).toInt();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->operateId = obj.value(JsonKey::key(JsonKey::OperateId)).toString();

    RSingleton<DataProcess>::instance()->processRelationOperate(db,socketId,request);
}

void DataParse::onProcessAddFriend(Database * db,int socketId,QJsonObject &obj)
{
    AddFriendRequest * request = new AddFriendRequest;
    request->stype = (SearchType)obj.value(JsonKey::key(JsonKey::AddType)).toInt();
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->operateId = obj.value(JsonKey::key(JsonKey::OperateId)).toString();

    RSingleton<DataProcess>::instance()->processAddFriend(db,socketId,request);
}

void DataParse::onProcessFriendListOperate(Database * db,int socketId,QJsonObject &obj)
{
    FriendListRequest * request = new FriendListRequest;
    request->accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();

    RSingleton<DataProcess>::instance()->processFriendList(db,socketId,request);
}

void DataParse::onProcessGroupingFriend(Database *db, int socketId, QJsonObject &dataObj)
{
    GroupingFriendRequest * request = new GroupingFriendRequest;
    request->type = (OperateGroupingFriend) dataObj.value(JsonKey::key(JsonKey::Type)).toInt();
    request->groupId = dataObj.value(JsonKey::key(JsonKey::GroupId)).toString();
    request->oldGroupId = dataObj.value(JsonKey::key(JsonKey::OldGroupId)).toString();
    request->stype = (SearchType)dataObj.value(JsonKey::key(JsonKey::SearchType)).toInt();

    QJsonObject simpleObj = dataObj.value(JsonKey::key(JsonKey::Users)).toObject();
    request->user.accountId = simpleObj.value(JsonKey::key(JsonKey::AccountId)).toString();
    request->user.nickName = simpleObj.value(JsonKey::key(JsonKey::NickName)).toString();
    request->user.signName = simpleObj.value(JsonKey::key(JsonKey::SignName)).toString();
    request->user.face = simpleObj.value(JsonKey::key(JsonKey::Face)).toInt();
    request->user.customImgId = simpleObj.value(JsonKey::key(JsonKey::FaceId)).toString();
    request->user.remarks = simpleObj.value(JsonKey::key(JsonKey::Remark)).toString();
    request->user.status = (OnlineStatus)simpleObj.value(JsonKey::key(JsonKey::Status)).toInt();

    RSingleton<DataProcess>::instance()->processGroupingFriend(db,socketId,request);
}

void DataParse::onProcessGroupingOperate(Database * db,int socketId,QJsonObject &obj)
{
    GroupingRequest * request = new GroupingRequest;
    request->uuid = obj.value(JsonKey::key(JsonKey::Uuid)).toString();
    request->groupId = obj.value(JsonKey::key(JsonKey::GroupId)).toString();
    request->gtype = (GroupingType)obj.value(JsonKey::key(JsonKey::GroupType)).toInt();
    request->type = (OperateGrouping)obj.value(JsonKey::key(JsonKey::Type)).toInt();
    request->groupName = obj.value(JsonKey::key(JsonKey::GroupName)).toString();

    RSingleton<DataProcess>::instance()->processGroupingOperate(db,socketId,request);
}

void DataParse::onProcessFileRequest(Database *db, int socketId, RBuffer &obj)
{
    FileItemRequest * request = new FileItemRequest();

    int control,itemType;
    if(!obj.read(control))
        return;
    request->control = (FileTransferControl)control;

    if(!obj.read(itemType))
        return;
    request->itemType = (FileItemType)itemType;

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
void DataParse::onProcessFileControl(Database *db, int socketId, RBuffer &obj)
{
    SimpleFileItemRequest * request = new SimpleFileItemRequest();
    int controlType = 0;
    if(!obj.read(controlType))
        return;
    request->control = static_cast<FileTransferControl>(controlType);

    int itemType = 0;
    if(!obj.read(itemType))
        return;
    request->itemType = static_cast<FileItemType>(itemType);

    if(!obj.read(request->md5))
        return;

    if(!obj.read(request->fileId))
        return;

    RSingleton<DataProcess>::instance()->processFileControl(db,socketId,request);
}

void DataParse::onProcessFileData(Database *db, int socketId, RBuffer &obj)
{
    FileDataRequest * request = new FileDataRequest();
    request->control =  T_DATA;

    if(!obj.read(request->md5))
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
