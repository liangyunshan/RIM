#include "wrapfactory.h"

#include "basemsgwrap.h"
#include "localmsgwrap.h"

WrapFactory::WrapFactory():msgWrap(nullptr)
{

}

MsgWrap *WrapFactory::getMsgWrap()
{
#ifdef __LOCAL_CONTACT__
    if(!msgWrap)
        msgWrap = new LocalMsgWrap();
#else
    if(!msgWrap)
        msgWrap = new BaseMsgWrap();
#endif

    return msgWrap;
}

