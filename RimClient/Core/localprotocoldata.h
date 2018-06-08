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

namespace QDB21{
struct QDB21{
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

struct QDB21_2051{
    unsigned long ulPackageLen;
    unsigned short usDestSiteNo;
    unsigned long ulDestDeviceNo;
    char cFileType;
    char cFilenameLen;
    char cFilename;
    char cFileData;
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
#define WM_DATA_AFFIRM      0
#define WM_DATA_NOAFFIRM    1
#define WM_TRANS_AFFIRM     2
#define WM_DATA_REG         3
#define WM_TEST             4
#define WM_TEST_RESULT      5
#define WM_STREAM_CTRL      6
#define WM_SENDREC_CTRL     7
#define WM_CONNECT_TEST     8
#define WM_UNROUTE          12
#define WM_ROUTE_BLOCK      13
#define WM_UNREGISTER       14
#define WM_SERVER_BUSY      21

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


#endif // LOCALPROTOCOLDATA_H
