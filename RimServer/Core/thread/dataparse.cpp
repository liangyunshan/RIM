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
