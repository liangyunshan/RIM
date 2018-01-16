#include "iopacket.h"

namespace ServerNetwork {

IOPacket *IOPacket::create(SOCKET sock, const char *src, const int length)
{
    IOPacket * packet = new IOPacket();
    packet->m_sock = sock;
    packet->m_size = length;

    memset(packet->m_buff,0,length + 1);
    memcpy(packet->m_buff,src,length);

    return packet;
}

void IOPacket::destory(IOPacket *packet)
{
    if(packet == NULL)
    {
        return;
    }

    delete packet;
}

IOPacket::IOPacket():m_sock(INVALID_SOCKET),m_size(0)
{

}

} //namespace ServerNetwork
