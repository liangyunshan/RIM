#include "dataparse.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "dataprocess.h"
#include "protocoldata.h"

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
        switch(obj.value(context.msgType).toInt())
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
    switch(obj.value(context.msgCommand).toInt())
    {
         case MSG_USER_LOGIN:
                                          onProcessUserLogin(db,socketId,obj.value(context.msgData).toObject());
                                          break;
         default:
             break;
    }
}

void DataParse::onProcessUserLogin(Database * db,int socketId,QJsonObject &obj)
{
    LoginRequest * request = new LoginRequest;
    request->accountName = obj.value("name").toString();
    request->password = obj.value("pass").toString();
    request->status = (OnlineStatus)obj.value("status").toInt();

    RSingleton<DataProcess>::instance()->processUserLogin(db,socketId,request);
}
