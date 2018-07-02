#ifndef NETWORK_GLOBAL_H
#define NETWORK_GLOBAL_H

#include <QtCore/qglobal.h>
#include <functional>

#if defined(Q_OS_WIN)
#include <WinSock2.h>
#endif

#if defined(NETWORK_LIBRARY)
#  define NETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace ServerNetwork {
class IocpContext;
}

typedef int(*Func)(const char *,const int);
typedef std::function<void(QByteArray &)> ByteArrayHandler;
typedef std::function<bool(int,ServerNetwork::IocpContext *,DWORD &)> IocpContextSender;
typedef std::function<int(int,const char*,int)> ByteSender;


#endif // NETWORK_GLOBAL_H
