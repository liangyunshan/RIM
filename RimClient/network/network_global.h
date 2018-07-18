#ifndef NETWORK_GLOBAL_H
#define NETWORK_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QByteArray>
#include <QLinkedList>

#if defined(NETWORK_LIBRARY)
#  define NETWORKSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NETWORKSHARED_EXPORT Q_DECL_IMPORT
#endif

#define SEND_MAGIC_NUM 0x7DBCD6AC               //发送魔数
#define RECV_MAGIC_NUM 0xD7CB6DCA               //接收魔数

#define MAX_PACKET 1024                         //发送数据时一次最大数据长度(不包括前后的控制信息)
#define MAX_SEND_BUFF (MAX_PACKET + 24)

#include <functional>

#pragma pack(push)
#pragma pack(1)

typedef int(*Func)(const char *,const int);

static unsigned int SERIALNO_STATIC = 1;

struct DataPacket
{
    unsigned int magicNum;                   //魔数
    unsigned int packId;                     //数据包身份，一个id可对应多个帧，也可以有一个帧
    unsigned int currentIndex;               //当前帧
    unsigned int totalIndex;                 //总帧数量
    unsigned int currentLen;                 //当前数据长度
    unsigned int totalLen;                   //总数据长度
};

//数据包接收缓冲
struct PacketBuff
{
    PacketBuff()
    {
        totalPackIndex = 0;
        recvPackIndex = 0;
        recvSize = 0;
        isCompleted = false;
    }

    /*!
     * @brief 获取缓存接收的分段数据
     * @note 将缓存接收的数据重新组装，拼接成新的数据；并在拼接后的数据头部插入协议头(21+2051) \n
     * @param[in] container 数据容器
     * @param[in] perPacketOffset 截取每个分段的起始点
     */
    void packDataWidthPrtocol(QByteArray & container,int perPacketOffset){
        if(isCompleted && recvSize > 0)
        {
            if(buff.size() > 0){
                container.append(buff.takeFirst());
            }

            while(!buff.isEmpty())
            {
                container.append(buff.takeFirst().mid(perPacketOffset));
            }
        }
    }

    QByteArray getFullData()
    {
        QByteArray data;
        if(isCompleted && recvSize > 0)
        {
            while(!buff.isEmpty())
            {
                data.append(buff.takeFirst());
            }
        }
        return data;
    }

    bool isCompleted;                           //该缓冲数据是否完整
    unsigned int recvSize;                      //接收数据的长度
    unsigned short totalPackIndex;              //总数据包分包大小
    unsigned short recvPackIndex;               //已经接收到数据的索引，在recvPackIndex==totalPackIndex时由处理线程进行重新组包

    qint64 totalPackLen;                        /*!< 总数据长度(分片数量*495+数据部分) */

    QLinkedList<QByteArray> buff;               //存放接收到数据(不包含网络数据头DataPacket)，插入时recvPackIndex+1
};

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
};
#define QDB495_SendPackage_Length sizeof(QDB495_SendPackage)
}

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
    char cFilenameLen;    //在文件类型为0 和 1 时，由ASCII字符、汉字等组成的文本信息内容；在其他情况下，可以理解为二进制数据流
};

#define QDB2051_Head_Length sizeof(QDB2051_Head)

/*!
 *  @brief  2051协议中文件类型
 */
enum FileType{
    F_NO_SUFFIX = 0,    /*!< 无文件后缀 */
    F_TEXT = 1,         /*!< 文本文件 */
    F_BINARY = 2        /*!< 二进制文件*/
};
}

namespace QDB2048{

struct QDB2048_Head{
    unsigned long ulPackageLen;
    unsigned short usDestSiteNo;
    unsigned long ulDestDeviceNo;
    unsigned short usSerialNo;
    unsigned short usOrderNo;
    unsigned short usErrorType;
};
#define QDB2048_Head_Length sizeof(QDB2048_Head)
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

/*!
 *  @brief 正文描述类型
 *  @details 5148等等
 */
enum OrderNoType{
    O_2051 = 2051,     /*!<  */
    O_2048 = 2048     /*!<  */
};

/*!
 *  @brief 传输方式
 *  @details 数据从本机出去的方式包含TCP、UDP、总线，至于北斗等方式，也是UDP传输至中间某一台位后，在转发。
 */
enum CommMethod{
    C_NONE,     /*!< 错误方式*/
    C_UDP,      /*!< UDP方式 */
    C_TCP,      /*!< TCP方式 */
    C_BUS       /*!< 总线方式 */
};

/*!
 *  @brief 对数据进行协议组包的时候，必须要用到的外部描述信息集合
 *  @details 填写协议头部信息时，像包长这类的字段信息可以实时获取，但是像站点等类型的信息的无法在实时组包的时候获取
 *  @author shangchao
 */
struct ProtocolPackage
{
    unsigned int uuid;              /*!< 文件发送时，当作文件id */
    unsigned short wSourceAddr;     /*!< 本节点号 */
    unsigned short wDestAddr;       /*!< 目标节点号 */
    unsigned char bPackType;        /*!< 报文类型 */
    unsigned char bPeserve;         /*!< 495保留字，用于扩展内部状态控制 */
                                    /*!< 0X00 标准正文 0X80 自有格式，暂为json格式*/
    unsigned short usSerialNo;      /*!< 流水号495*/
    unsigned short usOrderNo;       /*!< 协议号2051/2048*/
    unsigned short wOffset;         /*!< 数据帧偏移量 */
    unsigned long dwPackAllLen;     /*!< 总数据包大小(每一片数据*分片数量) */
    int cDate;                      /*!< 日期，4字节 */
    int cTime;                      /*!< 时间 低3字节 */
    char cFileType;                 /*!< 正文文件类型  0无文件后缀，1文本文件，2二进制文件 */
    QByteArray cFilename;           /*!< 文件名 如果发送的是文件，填写文件名，在接收完成时文件还原为该名称 */
    QByteArray data;                /*!< 正文内容 */

    ProtocolPackage()
    {
        wSourceAddr = 0;
        wDestAddr = 0;
        bPackType = 0;
        cFileType = 0;
        bPeserve = 0;
        usSerialNo = 0;
        usOrderNo = 2;
        cDate = 0;
        cTime = 0;
        wOffset = 0;
        dwPackAllLen = 0;
    }

    ProtocolPackage(QByteArray dataArray){
        this->data = dataArray;
    }

    ProtocolPackage operator=(const ProtocolPackage & package)
    {
        this->wSourceAddr = package.wSourceAddr;
        this->wDestAddr = package.wDestAddr;
        this->bPackType = package.bPackType;
        this->cFileType = package.cFileType;
        this->cFilename = package.cFilename;
        this->bPeserve = package.bPeserve;
        this->usSerialNo = package.usSerialNo;
        this->usOrderNo = package.usOrderNo;
        this->wOffset = package.wOffset;
        this->dwPackAllLen = package.dwPackAllLen;
        this->cDate = package.cDate;
        this->cTime = package.cTime;
        this->data = package.data;
        this->cDate = package.cDate;
        this->cTime = package.cTime;
        return *this;
    }
};

/*!
 *  @brief  使用495传输协议时，用于表示每种传输的类型，此类型由网络层根据495协议中选择设置
 */
enum PacketType_495{
    T_DATA_AFFIRM = 0,           /*!< 需要信息回执 */
    T_DATA_NOAFFIRM = 1,         /*!< 不需要信息回执 */
    T_TRANS_AFFIRM = 2,
    T_DATA_REG = 3,              /*!< 注册、注销 */
    T_TEST = 4,
    T_TEST_RESULT = 5,
    T_STREAM_CTRL = 6,
    T_SENDREC_CTRL = 7,
    T_CONNECT_TEST = 8,          /*!< 终端空闲时，发送给服务器请求测试自身是否在线 */
    T_UNROUTE = 12,              /*!< 路由不存在 */
    T_ROUTE_BLOCK = 13,          /*!< 路由不通 */
    T_UNREGISTER = 14,           /*!< 对端未注册 */
    T_SERVER_BUSY = 21,          /*!< 服务器忙 */
};

/*!
 *  @brief  接收时由网络层提取出的信息字段，该结构信息可用于应用层进行数据处理
 *  @note   需要由网络层向应用层传递数据时，可在此结构体内添加字段，并在网络层中将对应的信息赋值即可。
 */
struct ExtendData
{
    ExtendData():usSerialNo(0),usOrderNo(0),wOffset(0),dwPackAllLen(0)
    ,sliceNum(0){}
    CommMethod method;              /*!< 数据接收链路 */
    PacketType_495 type495;         /*!< 95信息类型 */
    unsigned char bPeserve;         /*!< 95保留字 */
    unsigned short usSerialNo;      /*!< 流水号 */
    unsigned short usOrderNo;       /*!< 编码代号 */
    unsigned short wOffset;         /*!< 分片序号 */
    unsigned long dwPackAllLen;     /*!< 数据总长度(分片数量*495头+数据长度) */
    unsigned short sliceNum;        /*!< 分片数量 */
};

/*!
 *  @brief 数据接收处理单元
 *  @details 由接收线程根据网络层传输协议，将分包的数据组装完毕后，并将网络层附带信息补充完整，交由应用层处理。 \n
 *           目前支持两种网络层传输控制协议：
 *              1.DataPacket;
 *              2.QDB495_SendPackage
 */
struct RecvUnit
{
    ExtendData extendData;      /*!< 可扩充数据信息，包含网络层相关信息 */
    QByteArray data;            /*!< 请求数据内容，已经去除网络层( @link DataPacket @endlink )数据头 */
};

typedef std::function<void(RecvUnit &)> DataHandler;

/*!
 *  @brief 待发送数据单元描述，包含了发送时使用的方式，以及待发送的数据
 */
struct SendUnit
{
    CommMethod method;              /*!< 发送方式 */
    ProtocolPackage dataUnit;       /*!< 待发送数据包及描述 */
};

/*!
 * @brief 文件数据发送过程
 * @note 当数据真正从网络发送出去之后，再更新发送的进度
 */
struct FileDataSendProgress
{
    unsigned short wSerialNo;       /*!< 流水号 */
    unsigned short wDestAddr;       /*!< 目标节点号 */
    unsigned short wPackLen;        /*!< 当前数据包大小 */
    unsigned long dwPackAllLen;     /*!< 总数据包大小 */
};

#pragma pack(pop)

#endif // NETWORK_GLOBAL_H
