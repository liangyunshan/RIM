#ifndef HEAD_H
#define HEAD_H

#include <QtGlobal>

#if defined(Q_OS_WIN)

#include <WinSock2.h>
#include <Windows.h>

#define IO_BUFF_SIZE  1024
#define MAX_RECV_SIZE 1024*8                    //接收缓冲区长度


#endif //Q_OS_WIN

#endif // HEAD_H
