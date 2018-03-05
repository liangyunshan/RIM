#include "SharedIocpData.h"

namespace ServerNetwork {

SharedIocpData::SharedIocpData():
    m_iocpContext(NULL)
{

}

void SharedIocpData::crateIocpContext()
{
    if(!m_iocpContext)
    {
        TcpClient * client = TcpClient::create();
        m_iocpContext =  IocpContext::create(IocpType::IOCP_ACCPET,client);
    }
}

}//namespace ServerNetwork
