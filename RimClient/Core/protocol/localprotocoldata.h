/*!
 *  @brief     通讯控制协议
 *  @details   定义网络通信协议
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef LOCALPROTOCOLDATA_H
#define LOCALPROTOCOLDATA_H

#pragma pack(push)
#pragma pack(1)

namespace QDB61A {

struct QDB61A_InterfaceCore{
    unsigned char ucPackageMarking;
    unsigned char ucPackageLength;
    unsigned short usMainBodyLength;
    unsigned char ucFormatType;
};


struct QDB61A_VersionExtensionArea_2{
    unsigned char ucActivityTypeMarking;
    unsigned short usSenderSysMarking;
    unsigned short usSenderHostMarking;
};

//报头-版本扩展区域,扩充集3，64位，8字节
struct QDB61A_VersionExtensionArea_3{
    unsigned short usSenderBodyMarking;
    unsigned char usTimeCode[5];
    unsigned char usReserve;
};

//1.1版本
struct QDB61A_1_1{
    QDB61A_InterfaceCore interfaceCore;
                                        //后接正文
};

//1.2版本
struct QDB61A_1_2{
    QDB61A_InterfaceCore interfaceCore;
    QDB61A_VersionExtensionArea_2 extensionArea_2;
                                        //后接正文
};

//1.3版本
struct QDB61A_1_3{
    QDB61A_InterfaceCore interfaceCore;
    QDB61A_VersionExtensionArea_2 extensionArea_2;
    QDB61A_VersionExtensionArea_3 extensionArea_3;
                                        //后接正文
};

}

namespace GJB4908{

const int MAX_PACKAGE_SIZE = 1000;

//[1]以下部分为传输区格式
struct GJB4908_UDP_PublicTransHeader{
    unsigned char ucVersion;
    unsigned char ucFlagAndType;
    unsigned short usPackageLen;
    unsigned short usPackageNo;
    unsigned char ucReserve;
    unsigned short usSenderSysFlag;
    unsigned short usSenderHostFlag;
    unsigned short usSenderEntityFlag;
    unsigned short usRecverSysFlag;
    unsigned short usRecverHostFlag;
    unsigned short usRecverEntityFlag;
                                        //后接报文区域,长度变长,用户的报文数据
};//19个字节
//[~1]传输区格式

//[2]以下部分为报文区格式
struct GJB4908_UDP_MessageArea{
    unsigned char ucMessMarking;
    unsigned char ucHeaderLength;
    unsigned short usMessLength;
    unsigned char ucMessCodeType;
    unsigned char ucActiveTypeMarking;
    unsigned short usSenderSysFlag;
    unsigned short usSenderHostFlag;
    unsigned short usSenderEntityFlag;
    unsigned char ucTimeCode[5];
    unsigned char ucMode;
                                        //后接正文,长度变长
};//18字节
//[~2]报文区格式

//[3]完整4908头报文
//UDP不分组报文头
struct GJB4908_UDP_NotSwitchGroup{
    GJB4908_UDP_PublicTransHeader   transHeader;    //传输区
    GJB4908_UDP_MessageArea         messageArea;    //报文区
};//19个字节

//UDP分组报文头-首报文
struct GJB4908_UDP_HeadGroup{
    GJB4908_UDP_PublicTransHeader   transHeader;    //传输区
    unsigned short                  ucPackageTotleNum; //传输区-用户报文分组数：用户报文需要拆分传输的分组总数
    GJB4908_UDP_MessageArea         messageArea;    //报文区
                                                    //后接报文区域,长度变长,用户的报文数据
};//21字节

//UDP分组报文头-后续报文
struct GJB4908_UDP_FollowGroup{
    GJB4908_UDP_PublicTransHeader   transHeader;    //传输区
    unsigned short                  ucPackageSwitchNum; //传输区-用户报文分组序号：同一用户报文的分组流水号
    GJB4908_UDP_MessageArea         messageArea;    //报文区
                                                    //后接报文区域,长度变长,用户的报文数据
};//21字节
//[~3]完整4908头报文
}

#pragma pack(pop)

#endif // LOCALPROTOCOLDATA_H
