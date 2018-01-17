#include "dataprocess.h"

#include <QDebug>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "messdiapatch.h"
#include "protocoldata.h"
using namespace ProtocolType;

DataProcess::DataProcess()
{

}

void DataProcess::proRegistResponse(QJsonObject data)
{
    RegistResponse response;
    if(data.value(context.msgStatus).toInt() == REGISTER_SUCCESS)
    {
        response.accountId = data.value("accountId").toString();
        MessDiapatch::instance()->onRecvRegistResponse(REGISTER_SUCCESS,response);
    }
    else
    {
        MessDiapatch::instance()->onRecvRegistResponse(REGISTER_SUCCESS,response);
    }
}

void DataProcess::proLoginResponse(QJsonObject data)
{

}
