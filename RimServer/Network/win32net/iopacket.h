/*!
 *  @brief     数据缓冲区
 *  @details   保存待发送数据缓冲区，包含缓冲区数据、长度以及目的socket
 *  @file      iopacket.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.13
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef IOPACKET_H
#define IOPACKET_H

#include "../head.h"
#include "../network_global.h"

namespace ServerNetwork {

class NETWORKSHARED_EXPORT IOPacket
{
public:
    static IOPacket * create(SOCKET sock,const char * src,const int length);
    static void destory(IOPacket * packet);

    SOCKET getSock(){return m_sock;}
    int getSize(){return m_size;}
    char * getBuff(){return m_buff;}

private:
    IOPacket();

private:
    SOCKET m_sock;
    unsigned short m_size;
    char m_buff[IO_BUFF_SIZE];
};

} //namespace ServerNetwork

#endif // IOPACKET_H
