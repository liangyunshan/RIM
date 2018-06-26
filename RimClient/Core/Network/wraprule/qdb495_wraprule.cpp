#include "qdb495_wraprule.h"
#include <QTime>
#include <QDebug>

#ifdef __LOCAL_CONTACT__
#include "localprotocoldata.h"
using namespace QDB495;

QDB495_WrapRule::QDB495_WrapRule():WrapRule()
{

}

QByteArray QDB495_WrapRule::wrap(const ProtocolPackage &package)
{
    QDB495_SendPackage SendPackage;
    memset(&SendPackage,0,sizeof(QDB495_SendPackage));

    SendPackage.bVersion = 1;
    SendPackage.bPackType = WM_DATA_NOAFFIRM;
    SendPackage.wPackLen = package.cFileData.size();
    SendPackage.wOffset = 1;
    SendPackage.dwPackAllLen = package.cFileData.size();
    SendPackage.wDestAddr = package.wDestAddr;
    SendPackage.wSourceAddr = package.wSourceAddr ;
    SendPackage.wSerialNo = QTime::currentTime().msecsTo(QTime(0,0,0,0))
                            +SendPackage.wDestAddr
                            +SendPackage.wSourceAddr;


    QByteArray ddsdata;
    ddsdata.append((char*)&SendPackage,sizeof(QDB495_SendPackage));
    ddsdata.append(package.cFileData);
    return ddsdata;
}

ProtocolPackage QDB495_WrapRule::unwrap(const QByteArray &data)
{
    QDB495_SendPackage SendPackage;
    memcpy(&SendPackage,data.data(),QDB495_SendPackage_Length);
    QByteArray tempdata = data.right(data.size() - QDB495_SendPackage_Length);

    ProtocolPackage package;
    package.cFileData = tempdata;
    package.bPackType = SendPackage.bPackType;
    package.wDestAddr = SendPackage.wDestAddr;
    package.wSourceAddr = SendPackage.wSourceAddr;
    return package;
}

/*

1.装备在启动时是否是一定会去主动链接指挥所?
2.主动请求注册是否一定是TCP方式?
3.如果使用TCP没有注册成功,客户端的好友界面还需要继续显示吗?

*/


#endif
