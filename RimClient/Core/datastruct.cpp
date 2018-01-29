#include "datastruct.h"

#include "util/rutil.h"

#include <QMetaType>

QDataStream& operator<<(QDataStream & stream,const UserInfoDesc & desc)
{
    stream<<desc.userName<<desc.loginState<<desc.originPassWord<<desc.password<<desc.isRemberPassword<<desc.isAutoLogin
         <<desc.isSystemPixMap
         <<desc.pixmap;
    return stream;
}


QDataStream& operator>>(QDataStream & stream,UserInfoDesc & desc)
{
    stream>>desc.userName>>desc.loginState>>desc.originPassWord>>desc.password>>desc.isRemberPassword>>desc.isAutoLogin
            >>desc.isSystemPixMap>>desc.pixmap;
    return stream;
}
