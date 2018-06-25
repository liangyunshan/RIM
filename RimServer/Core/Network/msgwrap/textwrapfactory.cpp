#include "textwrapfactory.h"

#include "json_wrapformat.h"

TextWrapFactory::TextWrapFactory():msgWrap(nullptr)
{

}

MsgWrap *TextWrapFactory::getTextWrap()
{
    if(msgWrap == nullptr)
        msgWrap = new Json_WrapFormat();

    return msgWrap;
}
