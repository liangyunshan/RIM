#include "dataprocess.h"

#include <QDebug>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "messdiapatch.h"
#include "jsonkey.h"

#include "protocoldata.h"
using namespace ProtocolType;

DataProcess::DataProcess()
{

}

void DataProcess::proRegistResponse(QJsonObject data)
{
    RegistResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == REGISTER_SUCCESS)
    {
        response.accountId = data.value(JsonKey::key(JsonKey::AccountId)).toString();
        MessDiapatch::instance()->onRecvRegistResponse(REGISTER_SUCCESS,response);
    }
    else
    {
        MessDiapatch::instance()->onRecvRegistResponse(REGISTER_SUCCESS,response);
    }
}

void DataProcess::proLoginResponse(QJsonObject data)
{
    LoginResponse response;


}
