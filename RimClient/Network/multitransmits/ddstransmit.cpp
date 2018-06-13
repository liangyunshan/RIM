#include "ddstransmit.h"
#include "DDS.h"
#include "Util/rlog.h"
#include <QDebug>

namespace ClientNetwork{

DDSTransmit::DDSTransmit() :
    BaseTransmit(C_BUS)
{
    int iRet=DDSInit("RIMDDSSOCKET",2);			//模块号
    iRet=DDSCreateTopic("CLIENTSEND",3);
    iRet=DDSCreateTopic("CLIENTRECV",4);

    iRet=DDSPublish(3,0,0);
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
           <<"DDSPublish CLIENTSEND："<<iRet
          <<"\n";
    iRet=DDSSubscribe(4,0,0);//主题
    if(iRet<=DTS_ERROR)
    {
        RLOG_ERROR("DDS系统集成服务订阅失败!");
        qDebug("DDS init fail:%d\n",iRet);
    }
    else
    {
        RLOG_INFO("DDS系统集成服务订阅成功,准备接收!");
        qDebug("DDS init SUCCESS:%d\n",iRet);
    }
}

DDSTransmit::~DDSTransmit()
{

}

bool DDSTransmit::startTransmit(const SendUnit &unit)
{
    QByteArray ddsdata = unit.data;
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
           <<""<<ddsdata
          <<"\n";
    int iRet=DDSSend(ddsdata.data(),ddsdata.size(),3);
    if (iRet<1)
    {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
               <<"发送主题CLIENTSEND报文失败 fail"<<iRet
              <<"\n";
        RLOG_ERROR("DDS发送主题CLIENTSEND报文失败!");
    }
    else
    {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"\n"
               <<"发送主题CLIENTSEND报文成功SUCC"<<iRet
              <<"\n";
    }
    return iRet;
}

bool DDSTransmit::startRecv(char *recvBuff, int recvBuffLen, std::function<void (QByteArray &)> recvDataFunc)
{
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int bwlen = sizeof(buf);
    unsigned int topic=0;
    int iRet = DDSFetch(buf,bwlen,topic);
    if(iRet==DTS_SUCCEED)
    {
        if(topic==4)
        {
            QByteArray recvDDS(buf,bwlen);
            recvDataFunc(recvDDS);
        }
        else
        {
            qDebug("收到非法数据");
        }
    }
    return true;
}

bool DDSTransmit::connect(const char *remoteIp, const unsigned short remotePort, int timeouts)
{
    return true;
}

bool DDSTransmit::close()
{
    return true;
}

}
