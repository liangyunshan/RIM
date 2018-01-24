#include "dataprocess.h"

#include <QDebug>
#include <QJsonDocument>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "messdiapatch.h"
#include "jsonkey.h"

#include "protocoldata.h"
using namespace ProtocolType;

DataProcess::DataProcess()
{

}

void DataProcess::proRegistResponse(QJsonObject & data)
{
    RegistResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == REGISTER_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            MessDiapatch::instance()->onRecvRegistResponse(REGISTER_SUCCESS,response);
        }
    }
    else
    {
        ResponseRegister rr = (ResponseRegister)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvRegistResponse(rr,response);
    }
}

void DataProcess::proLoginResponse(QJsonObject &data)
{
    LoginResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == LOGIN_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.baseInfo.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            response.baseInfo.nickName = dataObj.value(JsonKey::key(JsonKey::NickName)).toString();
            response.baseInfo.signName = dataObj.value(JsonKey::key(JsonKey::SignName)).toString();
            response.baseInfo.sexual = (Sexual)dataObj.value(JsonKey::key(JsonKey::Sexual)).toInt();
            response.baseInfo.birthday = dataObj.value(JsonKey::key(JsonKey::Birth)).toString();
            response.baseInfo.address = dataObj.value(JsonKey::key(JsonKey::Address)).toString();
            response.baseInfo.email = dataObj.value(JsonKey::key(JsonKey::Email)).toString();
            response.baseInfo.phoneNumber = dataObj.value(JsonKey::key(JsonKey::Phone)).toString();
            response.baseInfo.desc = dataObj.value(JsonKey::key(JsonKey::Desc)).toString();
            response.baseInfo.face = dataObj.value(JsonKey::key(JsonKey::Face)).toInt();
            response.baseInfo.customImgId = dataObj.value(JsonKey::key(JsonKey::FaceId)).toString();

            MessDiapatch::instance()->onRecvLoginResponse(LOGIN_SUCCESS,response);
        }
    }
    else
    {
        ResponseLogin rr = (ResponseLogin)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvLoginResponse(rr,response);
    }
}

void DataProcess::proUpdateBaseInfoResponse(QJsonObject &data)
{
    UpdateBaseInfoResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == UPDATE_USER_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.baseInfo.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            response.baseInfo.nickName = dataObj.value(JsonKey::key(JsonKey::NickName)).toString();
            response.baseInfo.signName = dataObj.value(JsonKey::key(JsonKey::SignName)).toString();
            response.baseInfo.sexual = (Sexual)dataObj.value(JsonKey::key(JsonKey::Sexual)).toInt();
            response.baseInfo.birthday = dataObj.value(JsonKey::key(JsonKey::Birth)).toString();
            response.baseInfo.address = dataObj.value(JsonKey::key(JsonKey::Address)).toString();
            response.baseInfo.email = dataObj.value(JsonKey::key(JsonKey::Email)).toString();
            response.baseInfo.phoneNumber = dataObj.value(JsonKey::key(JsonKey::Phone)).toString();
            response.baseInfo.desc = dataObj.value(JsonKey::key(JsonKey::Desc)).toString();
            response.baseInfo.face = dataObj.value(JsonKey::key(JsonKey::Face)).toInt();
            response.baseInfo.customImgId = dataObj.value(JsonKey::key(JsonKey::FaceId)).toString();

            MessDiapatch::instance()->onRecvUpdateBaseInfoResponse(UPDATE_USER_SUCCESS,response);
        }
    }
    else
    {
        ResponseUpdateUser rr = (ResponseUpdateUser)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvUpdateBaseInfoResponse(rr,response);
    }
}
