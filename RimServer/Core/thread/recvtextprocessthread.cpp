#include "recvtextprocessthread.h"

#include "Network/netglobal.h"
#include "Network/msgparse/msgparsefactory.h"
#include "Network/head.h"
#include "rsingleton.h"
#include "Network/tcpclient.h"

#include <QDebug>

RecvTextProcessThread::RecvTextProcessThread()
{

}

void RecvTextProcessThread::setDatabase(Database *db)
{
    database = db;
}

void RecvTextProcessThread::run()
{
    while(true)
    {
        while(G_RecvButts.size() == 0)
        {
            G_RecvCondition.wait(std::unique_lock<std::mutex>(G_RecvMutex));
        }

        bool existed = false;
        RecvUnit sockData;

        G_RecvMutex.lock();

        if(G_RecvButts.size() > 0)
        {
           sockData =  G_RecvButts.front();
           existed = true;
           G_RecvButts.pop();
        }

        G_RecvMutex.unlock();

//<<<<<<< HEAD
//#ifdef __LOCAL_CONTACT__
//        ProtocolPackage recvPack = RSingleton<QDB21_WrapRule>::instance()->unwrap(sockData.data);
//        ProtocolPackage recv2051Pack = RSingleton<QDB2051_WrapRule>::instance()->unwrap(recvPack.data);
//        sockData.data = recv2051Pack.data;
//        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
//               <<"sockData.data:"<<sockData.data<<"size"<<sockData.data.size()
//              <<"\n";
//        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(database,sockData);
//#else
//        RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(database,sockData);
//#endif
//=======
        if(existed)
            RSingleton<MsgParseFactory>::instance()->getDataParse()->processData(database,sockData);
//>>>>>>> d3ffafb480aea1647881162c1a402b43ef93005c
    }
}
