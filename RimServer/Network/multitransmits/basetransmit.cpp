#include "basetransmit.h"

namespace ServerNetwork{

BaseTransmit::BaseTransmit():netConnected(false)
{

}

BaseTransmit::~BaseTransmit()
{

}

bool BaseTransmit::connected()
{
    return netConnected;
}

}
