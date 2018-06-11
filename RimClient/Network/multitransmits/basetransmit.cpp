#include "basetransmit.h"

namespace ClientNetwork{

BaseTransmit::BaseTransmit(CommMethod method):netConnected(false),
    transMethod(method)
{

}

BaseTransmit::~BaseTransmit()
{

}

CommMethod BaseTransmit::type()
{
    return transMethod;
}

bool BaseTransmit::connected()
{
    return netConnected;
}

}
