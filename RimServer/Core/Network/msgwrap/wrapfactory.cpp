#include "wrapfactory.h"

#include "basemsgwrap.h"

WrapFactory::WrapFactory():msgWrap(nullptr)
{

}

MsgWrap *WrapFactory::getWrap()
{
    if(msgWrap == nullptr)
        msgWrap = new BaseMsgWrap();

    return msgWrap;
}
