#include "wrapfactory.h"

#include "basemsgwrap.h"

#ifdef __LOCAL_CONTACT__
#include "localmsgwrap.h"
#endif

WrapFactory::WrapFactory():msgWrap(nullptr)
{

}

MsgWrap *WrapFactory::getWrap()
{
#ifndef __LOCAL_CONTACT__
    if(msgWrap == nullptr)
        msgWrap = new BaseMsgWrap();
#else
    if(msgWrap == nullptr)
        msgWrap = new LocalMsgWrap();
#endif

    return msgWrap;
}
