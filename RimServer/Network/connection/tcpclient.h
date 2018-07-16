/*!
 *  @brief     客户端/客户端管理
 *  @details   封装了客户端的基础描述信息/记录所有连接的客户端信息，支持添加、删除
 *  @file      tcpclient.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.09
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QList>
#include <QString>
#include <QLinkedList>
#include <mutex>
#include <condition_variable>
#include <QHash>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QDataStream>
#include "../network_global.h"
#include "../head.h"

#include <QDebug>

namespace ServerNetwork {

//数据包接收缓冲
struct PacketBuff
{
    PacketBuff()
    {
        totalPackIndex = 0;
        totalPackLen = 0;
        recvPackIndex = 0;
        recvSize = 0;
        isCompleted = false;
    }

    ~PacketBuff(){

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

    bool isCompleted;                           /*!< 该缓冲数据是否完整 */
    qint64 recvSize;                            /*!< 接收数据的长度 */

    unsigned short totalPackIndex;              /*!< 总数据包分包大小 */
    unsigned short recvPackIndex;               /*!< 已经接收到数据的索引，在recvPackIndex==totalPackIndex时由处理线程进行重新组包 */

    qint64 totalPackLen;                        /*!< 总数据长度(分片数量*495+数据部分) */

    QLinkedList<QByteArray> buff;               /*!< 存放接收到数据(不包含网络数据头DataPacket)，插入时recvPackIndex+1 */
};

/*!
 *  @brief 文件传输过程中状态
 *  @details 用于底层控制接收文件的状态，状态由FILE_CREATE->FILE_OVER逐渐变化状态
 */
enum FileTransState
{
    FILE_ERROR,                     /*!< 错误状态， @note 默认状态 */
    FILE_TRANING,                   /*!< 文件传输过程中 */
    FILE_PAUSE,                     /*!< 文件传输暂停 */
    FILE_CANCEL,                    /*!< 文件传取消 */
    FILE_OVER                       /*!< 文件传输结束 */
};


/*!
 *  @brief  单个接收文本描述
 *  @attention 1.保存文件时需要指定文件的路径，此路径需要保存至数据库中，防止后期移动; @n
 *             2.文件使用md5进行重命名，取消文件的后缀，防止用户直接打开，真实的文件名保存在数据库中; @n
 */
struct FileRecvDesc
{
    FileRecvDesc():fileTransState(FILE_ERROR){}

    ~FileRecvDesc(){
        destory();
    }

    bool create(const QString& filePath)
    {
        if(!file.isOpen()){
            file.setFileName(filePath + "/" +md5);
            qDebug()<<"++++:"<<(filePath + "/" +md5);
            if(file.open(QFile::WriteOnly) )
            {
                if(file.resize(size))
                {
                    fileTransState = FILE_TRANING;
                    return true;
                }
            }
        }
        return false;
    }

    bool isNull(){return !file.isOpen();}

    bool seek(size_t pos)
    {
        if(!file.isOpen())
            return false;

        return file.seek(pos);
    }

    qint64 write(const QByteArray &data)
    {
        if(!file.isOpen())
            return -1;
        qint64 realWriteLen = file.write(data);
        writeLen += realWriteLen;
        return realWriteLen;
    }

    bool flush()
    {
        if(file.isOpen())
            return file.flush();
        return false;
    }

    bool isRecvOver()
    {
        return writeLen == file.size();
    }

    void close()
    {
        if(file.isOpen())
            file.close();
    }

    std::mutex & RWMutex(){return mutex;}

    void destory()
    {
        if(file.isOpen())
        {
            fileTransState = FILE_OVER;
            file.close();
        }
    }


    FileTransState state(){return fileTransState;}
    void setState(FileTransState state){fileTransState = state;}

    int itemType;                        /*!< 文件操作类型 @link FileItemType @endlink */
    int itemKind;                        /*!< 文件类型 @link FileItemKind @endlink */
    FileTransState fileTransState;       /*!< 文件传输状态，用于控制文件的状态 */
    qint64 size;                         /*!< 文件大小 */
    qint64 writeLen;                     /*!< 文件已经写入的大小 */
    QString fileName;                    /*!< 文件名称 @attention 维护文件真实的信息 */
    QString md5;                         /*!< 文件MD5 */
    QString fileId;                      /*!< 文件唯一标识，有客户端指定，与数据库中保持一致 */
    QString accountId;                   /*!< 自己ID */
    QString otherId;                     /*!< 接收方ID */
    QFile   file;                        /*!< 文件缓冲 */
    std::mutex mutex;                    /*!< 文件读写锁 */

#ifdef __LOCAL_CONTACT__
    int cdate;                           /*!< 日期 */
    int ctime;                           /*!< 时间 */
    unsigned short usSerialNo;           /*!< 流水号*/
    unsigned short usOrderNo;            /*!< 协议号*/
    unsigned char bPackType;             /*!< 报文类型 */
#endif
};

/*!
 *  @brief  单个发送文本描述
 *  @attention
 *
 */
struct FileSendDesc
{
    FileSendDesc():fileTransState(FILE_ERROR),readLen(0),size(0){
#ifdef __LOCAL_CONTACT__
        sliceNum = -1;
#endif
    }

    ~FileSendDesc(){
        destory();
    }

    bool open(){
        QString fullPath = filePath + QDir::separator() + md5;
        qDebug()<<"---:"<<(fullPath);
        if(QFileInfo(fullPath).exists()){
            file.setFileName(fullPath);
            return file.open(QFile::ReadOnly);
        }
        return false;
    }

    bool isNull(){return !file.isOpen();}

    bool seek(size_t pos)
    {
        if(!file.isOpen())
            return false;

        return file.seek(pos);
    }

    qint64 read(QByteArray &data){
        if(!file.isOpen())
            return -1;

        if(isSendOver())
            return -1;

        memset(readBuff,0,MAX_PACKET);
        qint64 realReadLen = file.read(readBuff,MAX_PACKET);
        data.append(readBuff,realReadLen);
#ifdef __LOCAL_CONTACT__
        sliceNum++;
#endif
        return readLen += realReadLen;
    }

    bool flush(){
        if(file.isOpen())
            return file.flush();
        return false;
    }

    void close(){
        if(file.isOpen())
            file.close();
    }

    bool isSendOver(){
        return readLen == size;
    }

    std::mutex & RWMutex(){return mutex;}

    void destory()
    {
        if(file.isOpen())
        {
            fileTransState = FILE_OVER;
            file.close();
        }
    }


    FileTransState state(){return fileTransState;}
    void setState(FileTransState state){fileTransState = state;}

    int itemType;                        /*!< 文件操作类型 @link FileItemType @endlink */
    int itemKind;                        /*!< 文件类型 @link FileItemKind @endlink */
    FileTransState fileTransState;       /*!< 文件传输状态，用于控制文件的状态 */
    qint64 size;                         /*!< 文件大小 */
    qint64 readLen;                      /*!< 文件已经发送的大小 */
    QString filePath;                    /*!< 文件保存的路径 */
    QString fileName;                    /*!< 文件名称 @attention 维护文件真实的信息 */
    QString md5;                         /*!< 文件MD5 */
    QString fileId;                      /*!< 文件唯一标识，有客户端指定，与数据库中保持一致 */
    QString accountId;                   /*!< 自己ID */
    QString otherId;                     /*!< 接收方ID */
    QFile   file;                        /*!< 文件缓冲 */
    std::mutex mutex;                    /*!< 文件读写锁 */
    char readBuff[MAX_PACKET];           /*!< 读取缓冲区 */

#ifdef __LOCAL_CONTACT__
    int sliceNum;                        /*!< 记录调用read次数，用于表示数据索引 */
    int cdate;                           /*!< 日期 */
    int ctime;                           /*!< 时间 */
    unsigned short usSerialNo;           /*!< 流水号*/
    unsigned short usOrderNo;            /*!< 协议号*/
    unsigned char bPackType;             /*!< 报文类型 */
    unsigned long dwPackAllLen;          /*!< 待发送文件总大小 */
#endif
};


class NETWORKSHARED_EXPORT TcpClient
{
public:
    static TcpClient * create();

    int socket() const {return cSocket;}

    void setIp(const char * destIp);
    QString ip() const {return QString(cIp);}

    void setPort(unsigned short cPort);
    unsigned short port() const {return cPort;}

    QHash<int,PacketBuff*> & getPacketBuffs(){return packetBuffs;}
    QByteArray & getHalfPacketBuff(){return halfPackBufff;}

    void setOnLineState(int val){onlineState = val;}
    int getOnLineState(){return onlineState;}

    void setAccount(QString id){accountId = id;}
    QString getAccount(){return accountId;}

    void setNickName(QString name){nickName = name;}
    QString getNickName(){return nickName;}

    std::mutex & BuffMutex(){return packBuffMutex;}

    int getPackId();

    bool addFile(QString fileId,FileRecvDesc * desc);
    bool removeFile(QString &fileId);
    FileRecvDesc * getFile(QString fileId);

private:
    explicit TcpClient();
    ~TcpClient();

    char cIp[32];
    unsigned short cPort;
    int cSocket;

    /*!
       @details 因为IOCP接收数据后，将数据从缓冲区拷贝至对应的缓冲区之后，才进行下一次的接收投递请求。
                这样即时断包，断的数据也是从头部截断，下一次读取数据后，发现头部不完整，则只需要从断包缓冲区将信息拿出拼接至此次数据头部即可。
    */
    QByteArray halfPackBufff;                       /*!< 非完整包缓冲区 */
    QHash<int,PacketBuff*> packetBuffs;             /*!< 多包缓冲区 */
    std::mutex packBuffMutex;
    std::mutex packIdMutex;
    int sendPackId;                                 /*!< 每次响应结果ID，可能被拆分成多个包，但每个子包的ID是一致的。 */

    int onlineState;                   /*!< 在线状态(与OnlineStatus保持一致) */
    QString accountId;                 /*!< 用户ID */
    QString nickName;                  /*!< 用户昵称 */

    QHash<QString,FileRecvDesc*> fileRecvList;      /*!< 文件接收缓冲列表 */
    std::mutex fileMutex;

    friend class TcpClientManager;
};

typedef std::list<TcpClient*> ClientList;

class NETWORKSHARED_EXPORT TcpClientManager
{
public:
    TcpClientManager();
    static TcpClientManager * instance();

    void addClient(TcpClient * client);

    void remove(TcpClient * client);
    void removeAll();

    TcpClient *  getClient(int sock);
    TcpClient *  getClient(QString accountId);
    ClientList   getClients(QString accountId);
    TcpClient *  addClient(int sockId, char* ip, unsigned short port);

    int counts();

private:
    static TcpClientManager * manager;

    std::list<TcpClient *> clientList;
    std::mutex mutex;
};

}   //namespace ServerNetwork

#endif // TCPCLIENT_H
