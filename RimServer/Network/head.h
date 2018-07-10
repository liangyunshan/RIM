/*!
 *  @brief     网络层协议文件
 *  @details   定义即时通讯软件网路层传输控制协议
 *  @file      head.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.02.23
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef HEAD_H
#define HEAD_H

#include <QByteArray>

#ifdef Q_OS_WIN
#include <WinSock2.h>
#include <Windows.h>
#endif

#define IO_BUFF_SIZE  1024
#define MAX_RECV_SIZE 1024*8

#define MAX_PACKET 1024                         /*!< 应用层数据最大发送长度，超过此长度后，网络层会进行拆包。 @attention 未加入网络层协议头， @link DataPacket @endlink */
#define MAX_SEND_BUFF (MAX_PACKET + 24)         /*!< 网络层最大发送长度 */

#define SOCK_CHAR_BUFF_LEN  32                  /*!< 存储ip地址集合长度 */

/*!
 *  @brief  接收/发送魔数
 *  @details 用于对接收数据的校验，服务器的接收与客户端的发送保持一致，服务器的发送与客户端的接收保持一致。
 */
#define RECV_MAGIC_NUM 0x7DBCD6AC
#define SEND_MAGIC_NUM 0xD7CB6DCA

/*!
 *  @brief 网络层数据传输控制头
 *  @details 在传输时网络层包裹了应用层的数据内容，用于即时通讯软件网络层交互时对大数据的包的分包传输以及组包，同时可对数据的正确性进行校验。
 */
struct DataPacket
{
    unsigned int magicNum;                   /*!< 魔数 @link RECV_MAGIC_NUM @endlink */
    unsigned int packId;                     /*!< 数据包身份，一个id至少对应一个数据帧。 */
    unsigned int currentIndex;               /*!< 当前帧索引 @attention 起始索引为0  */
    unsigned int totalIndex;                 /*!< 总帧数量 */
    unsigned int currentLen;                 /*!< 当前数据长度 */
    unsigned int totalLen;                   /*!< 总数据长度 */
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
#define MAX_495SEND_BUFF (MAX_PACKET+QDB495_SendPackage_Length)     /*!< 网络层最大发送长度 */

}

/*!
 *  @brief 协议类型
 *  @details 使用TCP传输时，因需要确认机制，因此发送的协议采用2051，确认协议采用2048.
 */
enum OrderNoType{
    O_2051 = 2051,      /*!<  */
    O_2048 = 2048       /*!<  */
};

/*!
 *  @brief 对数据进行协议组包的时候，必须要用到的外部描述信息集合
 *  @warning 【若此结构发生变动，同步修改SQL/Datatable/RChat716Cache类】
 *  @details 填写协议头部信息时，像包长这类的字段信息可以实时获取，但是像站点等类型的信息的无法在实时组包的时候获取
 *  @author shangchao
 */
struct ProtocolPackage
{
    unsigned short wSourceAddr;     /*!< 本节点号 */
    unsigned short wDestAddr;       /*!< 目标节点号 */
    unsigned char bPackType;        /*!< 报文类型 */
    unsigned char bPeserve;         /*!< 95保留字，用于扩展内部状态控制 */
                                    /*!< 0X00 标准正文 0X80 自有格式，暂为json格式*/
    unsigned short usSerialNo;      /*!< 流水号*/
    unsigned short usOrderNo;       /*!< 协议号*/
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
        bPeserve = 0;
        usSerialNo = 0;
        usOrderNo = 3;
        cFileType = 0;
        bPeserve = 0;
        cDate = 0;
        cTime = 0;
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
        this->data = package.data;
        this->cDate = package.cDate;
        this->cTime = package.cTime;
        return *this;
    }
};

/*!
 *  @brief 传输方式
 *  @details 数据从本机出去的方式包含TCP、UDP、总线，至于北斗等方式，也是UDP传输至中间某一台位后，再转发。
 */
enum CommMethod{
    C_NONE,     /*!< 错误方式*/
    C_UDP,      /*!< UDP方式 */
    C_TCP,      /*!< TCP方式 */
    C_BUS       /*!< 总线方式 */
};

/*!
 *  @brief 待发送数据单元描述，包含了发送时使用的方式，以及待发送的数据 \n
 *         ProtocolPackage可作为通用的数据发送格式，用户可在此结构体内部扩充信息。
 */
struct SendUnit
{
    SendUnit():localServer(true){}
    SOCKET sockId;                  /*!< 客户端Socket标识Id */
    bool localServer;               /*!< 数据信息目的指挥中心是否为当前节点，默认为true */
    CommMethod method;              /*!< 发送方式 */
    ProtocolPackage dataUnit;       /*!< 待发送数据包及描述 */
};

/*!
 *  @brief 数据操作类型
 */
enum SocketOperateType
{
    SOCKET_TEXT,                /*!< 文字信息 */
    SOCKET_FILE,                /*!< 文件信息 */
    SOCKET_IMG                  /*!< 图片信息 */
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
    SocketOperateType type;     /*!< 请求类型 */
    SOCKET sockId;              /*!< 客户端Socket标识Id */
    PacketType_495 type495;     /*!< 495信息类型 */
    unsigned char bPeserve;     /*!< 95保留字 */
    unsigned short usSerialNo;   /*!< 流水号 */
    unsigned short usOrderNo;    /*!< 编码代号 */
};

/*!
 *  @brief 数据处理单元
 *  @details 封装了客户端的每个请求，服务器处理线程从列表中取出每个单元，根据应用层传输协议进行相应处理。
 */
struct RecvUnit
{
    ExtendData extendData;      /*!< 可扩充数据信息，包含网络层相关信息 */
    QByteArray data;            /*!< 请求数据内容，已经去除网络层( @link DataPacket @endlink )数据头，但其它应用层的数据格式头没有去除 */
};

#endif // HEAD_H
