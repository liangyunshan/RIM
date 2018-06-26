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
 *  @brief 数据处理单元
 *  @details 封装了客户端的每个请求，服务器处理线程从列表中取出每个单元，根据应用层传输协议进行相应处理。
 */
struct SocketInData
{
    SocketOperateType type;     /*!< 请求类型 */
    SOCKET sockId;              /*!< 客户端Socket标识Id */
    QByteArray data;            /*!< 请求数据内容，已经去除网络层( @link DataPacket @endlink )数据头 */
};

/*!
 *  @brief 待发送数据单元
 *  @details 封装了响应客户端结果的数据内容，发送线程从队列中取出单元，加上网络层数据头，发送至目标网络
 */
struct SocketOutData
{
    SOCKET sockId;              /*!< 客户端Socket标识Id */
    QByteArray data;            /*!< 待发送数据内容，还未加上网络层( @link DataPacket @endlink )数据头 */
};

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
    char cFileType;                 /*!< 正文文件类型  0无文件后缀，1文本文件，2二进制文件 */
    QByteArray cFilename;           /*!< 文件名 如果发送的是文件，填写文件名，在接收完成时文件还原为该名称 */
    QByteArray cFileData;           /*!< 正文内容 */

    ProtocolPackage()
    {
        wSourceAddr = 0;
        wDestAddr = 0;
        bPackType = 0;
        cFileType = 0;
    }

    ProtocolPackage operator=(const ProtocolPackage & package)
    {
        this->bPackType = package.bPackType;
        this->cFileData = package.cFileData;
        this->cFilename = package.cFilename;
        this->cFileType = package.cFileType;
        this->wDestAddr = package.wDestAddr;
        this->wSourceAddr = package.wSourceAddr;
        return *this;
    }
};


#endif // HEAD_H
