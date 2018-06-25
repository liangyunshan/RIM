#include "basetransmit.h"

namespace ClientNetwork{

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
