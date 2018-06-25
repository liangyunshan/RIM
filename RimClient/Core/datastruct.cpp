#include "datastruct.h"

#include "util/rutil.h"

#include <QDebug>
#include <QMetaType>

QDataStream& operator<<(QDataStream & stream,const UserInfoDesc & desc)
{
    stream<<desc.userName<<desc.accountId<<desc.loginState<<desc.originPassWord<<desc.password<<desc.isRemberPassword<<desc.isAutoLogin
         <<desc.isSystemIcon
         <<desc.iconId;
    return stream;
}

QDataStream& operator>>(QDataStream & stream,UserInfoDesc & desc)
{
    stream>>desc.userName>>desc.accountId>>desc.loginState>>desc.originPassWord>>desc.password>>desc.isRemberPassword>>desc.isAutoLogin
            >>desc.isSystemIcon>>desc.iconId;
    return stream;
}
