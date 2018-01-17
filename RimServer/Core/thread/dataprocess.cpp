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

    if(RSingleton<SQLProcess>::instance()->processUserRegist(db,request,registId))
    {
        RegistResponse * response = new RegistResponse;

        response->accountId = registId;

        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,REGISTER_FAILED);
    }

    G_SendMutex.lock();
    G_SendButts.enqueue(data);
    G_SendMutex.unlock();

    G_SendCondition.wakeOne();
}

void DataProcess::processUserLogin(Database * db,int socketId, LoginRequest *request)
{
    SocketOutData data;
    data.sockId = socketId;

    if(RSingleton<SQLProcess>::instance()->processUserLogin(db,request))
    {
        LoginResponse * response = new LoginResponse;
        response->accountId = "12345";
        response->nickName = "yiluxiangbei";
        response->signName = "say something";
        response->sexual = MAN;
        response->birthday = 19920828;
        response->address = "china nanjing sifangxingcun";
        response->email = "407859345@qq.com";
        response->phoneNumber = "18012955183";
        response->desc = "Hello World";
        response->face = 1;

        data.data =  RSingleton<MsgWrap>::instance()->handleMsg(response);
    }
    else
    {
        data.data =  RSingleton<MsgWrap>::instance()->handleErrorSimpleMsg(request->msgType,request->msgCommand,LOGIN_UNREGISTERED);
    }

    G_SendMutex.lock();
    G_SendButts.enqueue(data);
    G_SendMutex.unlock();

    G_SendCondition.wakeOne();
}
