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

using namespace ProtocolType;

DataParse::DataParse(QObject *parent) : QObject(parent)
{

}

void DataParse::processData(Database *db,const SocketInData &data)
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

            default:
                  break;
        }
    }
    else
    {
        RLOG_ERROR("recv a error data,skip!");
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
        request->accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
        request->destAccountId = dataObj.value(JsonKey::key(JsonKey::DestId)).toString();
        request->type = (SearchType)dataObj.value(JsonKey::key(JsonKey::SearchType)).toInt();
        request->timeStamp = (SearchType)dataObj.value(JsonKey::key(JsonKey::Time)).toInt();
        request->sendData = dataObj.value(JsonKey::key(JsonKey::Data)).toString();

        RSingleton<DataProcess>::instance()->processText(db,socketId,request);
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
