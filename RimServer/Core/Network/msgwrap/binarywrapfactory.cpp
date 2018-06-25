#include "binarywrapfactory.h"

#include "binary_wrapformat.h"

BinaryWrapFactory::BinaryWrapFactory()
{

}

MsgWrap *BinaryWrapFactory::getTextWrap()
{
    if(msgWrap == nullptr)
        msgWrap = new Binary_WrapFormat();

    return msgWrap;
}
