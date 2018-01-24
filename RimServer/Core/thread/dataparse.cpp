#include "dataparse.h"

#include <QJsonDocument>
#include <QJsonParseError>
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

            default:
                  break;
        }
    }
    else
    {
        RLOG_ERROR("recv a error data,skip!");
    }
}

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
         default:
             break;
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
    request->baseInfo.desc = obj.value(JsonKey::key(JsonKey::Desc)).toString();
    request->baseInfo.face = obj.value(JsonKey::key(JsonKey::Face)).toInt();
    request->baseInfo.customImgId = obj.value(JsonKey::key(JsonKey::FaceId)).toString();

    RSingleton<DataProcess>::instance()->processUpdateUserInfo(db,socketId,request);
}
