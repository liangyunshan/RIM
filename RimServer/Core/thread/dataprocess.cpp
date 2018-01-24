#include "dataprocess.h"

#include <QDebug>

#include "Util/rutil.h"
#include "rsingleton.h"
#include "sql/sqlprocess.h"
#include "Network/msgwrap.h"
#include "Network/head.h"
#include "Network/netglobal.h"

DataProcess::DataProcess()
{

}

void DataProcess::processUserRegist(Database *db, int socketId, RegistRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    QString registId;

    ResponseRegister regResult = RSingleton<SQLProcess>::instance()->processUserRegist(db,request,registId);

    if(regResult == REGISTER_SUCCESS)
    {
        RegistResponse * response = new RegistResponse;

        response->accountId = registId;

        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,regResult);
    }

    delete request;

    G_SendMutex.lock();
    G_SendButts.enqueue(data);
    G_SendMutex.unlock();

    G_SendCondition.wakeOne();
}

void DataProcess::processUserLogin(Database * db,int socketId, LoginRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    ResponseLogin loginResult = RSingleton<SQLProcess>::instance()->processUserLogin(db,request);

    if(loginResult == LOGIN_SUCCESS)
    {
        LoginResponse * response = new LoginResponse;

        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->accountId,response->baseInfo);

        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,loginResult);
    }

    delete request;

    G_SendMutex.lock();
    G_SendButts.enqueue(data);
    G_SendMutex.unlock();

    G_SendCondition.wakeOne();
}

void DataProcess::processUpdateUserInfo(Database * db,int socketId, UpdateBaseInfoRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    ResponseUpdateUser updateResult = RSingleton<SQLProcess>::instance()->processUpdateUserInfo(db,request);

    if(updateResult == UPDATE_USER_SUCCESS)
    {
        UpdateBaseInfoResponse * response = new UpdateBaseInfoResponse;

        RSingleton<SQLProcess>::instance()->getUserInfo(db,request->baseInfo.accountId,response->baseInfo);
        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);

        delete response;
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,updateResult);
    }

    delete request;

    G_SendMutex.lock();
    G_SendButts.enqueue(data);
    G_SendMutex.unlock();

    G_SendCondition.wakeOne();
}
