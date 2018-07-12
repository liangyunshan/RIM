#include "ddstransmit.h"
#include "../3rdhead/DDS.h"
#include "Util/rlog.h"

namespace ClientNetwork{

DDSTransmit::DDSTransmit() :
    BaseTransmit()
{
    int iRet = DDSInit("RIMDDSSOCKET",2);			//模块号
    iRet = DDSCreateTopic("CLIENTSEND",3);
    iRet = DDSCreateTopic("CLIENTRECV",4);

    iRet = DDSPublish(3,0,0);
    iRet = DDSSubscribe(4,0,0);//主题

    if(iRet<=DTS_ERROR)
    {
        RLOG_ERROR("DDS init failed!");
        netConnected = false;
    }
    else
    {
        netConnected = true;
        RLOG_INFO("DDS init success!");
    }
}

DDSTransmit::~DDSTransmit()
{

}

CommMethod DDSTransmit::type()
{
    return C_BUS;
}

QString DDSTransmit::name()
{
    return "DDS";
}

bool DDSTransmit::startTransmit(SendUnit &unit)
{
    QByteArray ddsdata = unit.dataUnit.data;
    int iRet = DDSSend(ddsdata.data(),ddsdata.size(),3);
    if (iRet < 1){
        RLOG_ERROR("DDS send topic failed!");
    }

    return iRet;
}

bool DDSTransmit::startRecv(char *recvBuff, int recvBuffLen, DataHandler recvDataFunc)
{
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int bwlen = sizeof(buf);
    unsigned int topic=0;
    int iRet = DDSFetch(buf,bwlen,topic);

    if(iRet==DTS_SUCCEED)
    {
        if(topic == 4)
        {
            RecvUnit inData;
            inData.data = QByteArray(buf,bwlen);
            recvDataFunc(inData);
        }
        else
        {
            RLOG_ERROR("DDS recv invalid data!");
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
