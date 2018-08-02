/*!
 *  @brief     495传输控制协议
 *  @details   采用716传输控制协议报文头，实现TCP传输，解决TCP传输粘包问题
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef TCP495DATAPACKETRULE_H
#define TCP495DATAPACKETRULE_H

#include "wraprule.h"

#include <QHash>
#include <mutex>
#include <vector>

#define TCP_SEND_BUFF 4096

#ifdef __LOCAL_CONTACT__

namespace ClientNetwork{

/*!
 *  @brief 用于在多包接收数据时，用于信息的访问标识。
 */
struct RecvFileTypeId
{
    RecvFileTypeId():wSourceAddr(0),wDestAddr(0),usSerialNo(0){}
    RecvFileTypeId(unsigned short source,unsigned short dest,unsigned short serialNo,QDB2051::FileType ftype = QDB2051::F_NO_SUFFIX):wSourceAddr(source),
        wDestAddr(dest),usSerialNo(serialNo),filetype(ftype){}

    bool operator== (const RecvFileTypeId & others){
        if(this == &others)
            return true;

        return (this->wSourceAddr == others.wSourceAddr && this->wDestAddr == others.wDestAddr
                && this->usSerialNo == others.usSerialNo);
    }

    unsigned short wSourceAddr;     /*!< 源节点号 */
    unsigned short wDestAddr;       /*!< 目的节点号 */
    unsigned short usSerialNo;      /*!< 流水号 */
    QDB2051::FileType filetype;     /*!< 文件类型 */
};

class TCP495DataPacketRule  : public WrapRule
{
public:
    explicit TCP495DataPacketRule();

    void wrap(ProtocolPackage & data);
    bool unwrap(const QByteArray & data,ProtocolPackage & result);

    bool wrap(ProtocolPackage &dataUnit, std::function<int(const char *,const int)> sendDataFunc);
    bool unwrap(const char * data,const int length,DataHandler recvDataFunc);

private:
    bool recvData(const char *recvData, int recvLen);
    int countTotoalIndex(const int totalLength);

    bool addFileId(RecvFileTypeId fileId);
    void removeFileId(RecvFileTypeId fileId);
    bool queryFiletype(RecvFileTypeId &fileId, QDB2051::FileType & pType);

private:
    unsigned short SendPackId;
    char sendBuff[TCP_SEND_BUFF];

    QByteArray lastRecvBuff;              //断包接收缓冲区
    QHash<int,PacketBuff*> packetBuffs;   //多包缓冲区

    std::recursive_mutex fileTypeMutex;
    std::vector<RecvFileTypeId> fileTypeVecs;

    DataHandler dhandler;
};

}

#endif

#endif // TCP495DATAPACKETRULE_H
