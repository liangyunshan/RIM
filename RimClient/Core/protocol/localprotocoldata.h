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

#pragma pack(1)

namespace QDB21{
struct QDB21_Head{
    unsigned short usDestAddr;
    unsigned short usSourceAddr;
    unsigned long ulPackageLen;
    char cDate[4];
    char cTime[3];
    char cTypeNum;
    unsigned short usSerialNo;
    unsigned short usOrderNo;
                                    //后面是可变长数据
};//20字节长度

#define QDB21_Head_Length sizeof(QDB21_Head)
}

namespace QDB2051{
struct QDB2051_Head{
    unsigned long ulPackageLen;
    unsigned short usDestSiteNo;
    unsigned long ulDestDeviceNo;
    char cFileType;
    char cFilenameLen;
    char cFileNameData[0];
                                    //在文件类型为0 和 1 时，由ASCII字符、汉字等组成的文本信息内容；在其他情况下，可以理解为二进制数据流
};//信息类别号为2051的文本信息
}

namespace QDB205{

struct TK205_SendPackage{
    unsigned char bVersion;
    unsigned char source[4];
    unsigned short dest[4];
    unsigned char index[3];
    unsigned char time[6];
    unsigned char length[3];

    char cPackDataBuf[1];                //报文内容

    char endof;
};

}

namespace QDB495{

//报文类型
#define WM_DATA_AFFIRM      0   //需要信息回执
#define WM_DATA_NOAFFIRM    1   //不需要信息回执
#define WM_TRANS_AFFIRM     2
#define WM_DATA_REG         3   //注册、注销
#define WM_TEST             4
#define WM_TEST_RESULT      5
#define WM_STREAM_CTRL      6
#define WM_SENDREC_CTRL     7
#define WM_CONNECT_TEST     8   //终端空闲时，发送给服务器请求测试自身是否在线
#define WM_UNROUTE          12  //路由不存在
#define WM_ROUTE_BLOCK      13  //路由不通
#define WM_UNREGISTER       14  //对端未注册
#define WM_SERVER_BUSY      21  //服务器忙

struct QDB495_SendPackage{
    unsigned char bVersion;
    unsigned char bPackType;
    unsigned short wPackLen;
    unsigned char bPriority;
    unsigned char bPeserve;
    unsigned short wSerialNo;
    unsigned short wCheckout;
    unsigned short wOffset;
    unsigned long dwPackAllLen;
    unsigned short wDestAddr;
    unsigned short wSourceAddr;
    char cPackDataBuf[];
};

#define QDB495_SendPackage_Length sizeof(QDB495_SendPackage)

}

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

#pragma pack()

#endif // LOCALPROTOCOLDATA_H
