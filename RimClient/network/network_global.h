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

typedef int(*Func)(const char *,const int);

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

/*!
 *  @brief 对数据进行协议组包的时候，必须要用到的外部描述信息集合
 *  @details 填写协议头部信息时，像包长这类的字段信息可以实时获取，但是像站点等类型的信息的无法在实时组包的时候获取
 *  @author shangchao
 */
struct ProtocolPackage
{
    unsigned short wSourceAddr;     /*!< 本节点号 */
    unsigned short wDestAddr;       /*!< 目标节点号 */
    unsigned char bPackType;        /*!< 报文类型 */
    unsigned char bPeserve;         /*!< 495保留字，用于扩展内部状态控制 */
    char cFileType;                 /*!< 正文文件类型  0无文件后缀，1文本文件，2二进制文件 */
    QByteArray cFilename;           /*!< 文件名 如果发送的是文件，填写文件名，在接收完成时文件还原为该名称 */
    QByteArray data;                /*!< 正文内容 */

    ProtocolPackage()
    {
        wSourceAddr = 0;
        wDestAddr = 0;
        bPackType = 0;
        cFileType = 0;
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
        this->data = package.data;
        return *this;
    }
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
    PacketType_495 type495;     /*!< 495信息类型 */
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

#endif // NETWORK_GLOBAL_H
