#include "iocpcontext.h"

namespace ServerNetwork {

IocpContext::IocpContext()
{

}

IocpContext *IocpContext::create(IocpType type, TcpClient *client)
{
    IocpContext * context = new IocpContext;
    memset(context,0,sizeof(IocpContext));

    context->m_type = type;
    context->m_client = client;

    context->getWSABUF().buf = context->getPakcet();
    context->getWSABUF().len = MAX_RECV_SIZE - 1;

    return context;
}

void IocpContext::destory(IocpContext *context)
{
    if(context == NULL)
    {
        return;
    }

    context->m_client = NULL;

    delete context;
    context = NULL;

}

} // namespace ServerNetwork
