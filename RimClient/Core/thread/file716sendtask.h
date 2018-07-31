/*!
 *  @brief     处理716文件发送队列
 *  @details   接收发送文件的请求，控制发送的顺序，暂时同一时刻只发送一个文件。
 *  @author    wey
 *  @version   1.0
 *  @date      2018.07.10
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILE716SENDTASK_H
#define FILE716SENDTASK_H

#include "Network/rtask.h"
#include "../protocol/datastruct.h"
#include "Network/multitransmits/basetransmit.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

#include <list>
#include <mutex>
#include <condition_variable>
#include <memory>

/*!
 *  @brief  待发送文件管理器
 *  @note 接收所有准备发送的文件，并按照客户端节点的方式进行保存，每个节点的待发送文件均保存在queue之中 \n
 */
class FileSendManager
{
public:
    explicit FileSendManager();

    bool addFile(SenderFileDesc & fileInfo);
    bool deleteFile(SenderFileDesc & fileInfo);
    SenderFileDesc getFile();
    void pop_front();
    bool isEmpty();
    int size();

private:
    std::list<SenderFileDesc> fileList;
    std::mutex mutex;
};

/*!
 *  @brief  单个发送文本描述
 *  @attention
 *
 */
struct FileSendDesc
{
    FileSendDesc():size(0),readLen(0){
#ifdef __LOCAL_CONTACT__
        sliceNum = -1;
#endif
    }

    ~FileSendDesc(){
        destory();
    }

    bool parse(QString fullPath){
        QFileInfo fileInfo(fullPath);
        if(fileInfo.exists()){
            this->fullPath = fullPath;
            fileName = fileInfo.fileName();
            size = fileInfo.size();
#ifdef __LOCAL_CONTACT__
            dwPackAllLen = fileInfo.size();
#endif
            return true;
        }
        return false;
    }

    bool open(){
        file.setFileName(fullPath);
        return file.open(QFile::ReadOnly);
    }

    bool isNull(){return !file.isOpen();}

    bool seek(size_t pos){
        if(!file.isOpen())
            return false;
        return file.seek(pos);
    }

    qint64 read(QByteArray &data);

    bool isSendOver(){
        return readLen == size;
    }

    void close(){
        if(file.isOpen())
            file.close();
    }

    void destory(){
        if(file.isOpen()){
            file.close();
        }
    }

    int fileType;                        /*!< 文件操作类型 @link QDB2051::FileType @endlink */
    int itemKind;                        /*!< 文件类型 */
    qint64 size;                         /*!< 文件总大小 */
    qint64 readLen;                      /*!< 文件已经发送的大小 */
    QString fullPath;                    /*!< 文件本地全路径 */
    QString fileName;                    /*!< 文件名称 @attention 维护文件真实的信息，文件名由路径fullPath直接获得 */
    QString accountId;                   /*!< 自己ID */
    QString otherSideId;                 /*!< 接收方ID */
    QFile   file;                        /*!< 文件缓冲 */
    char readBuff[MAX_PACKET];           /*!< 读取缓冲区 */

#ifdef __LOCAL_CONTACT__
    int sliceNum;                        /*!< 记录调用read次数，用于表示数据索引 */
    int cdate;                           /*!< 日期 */
    int ctime;                           /*!< 时间 */
    unsigned short usSerialNo;           /*!< 流水号*/
    unsigned short usOrderNo;            /*!< 协议号*/
    unsigned char bPackType;             /*!< 报文类型 */
    unsigned long dwPackAllLen;          /*!< 待发送文件总大小 */
    ParameterSettings::CommucationMethod method;    /*!< 通信方式 */
    ParameterSettings::MessageFormat format;        /*!< 报文格式 */
#endif
};

#ifdef  __LOCAL_CONTACT__

class File716SendTask : public ClientNetwork::RTask
{
    Q_OBJECT
public:
    explicit File716SendTask(QObject *parent = Q_NULLPTR);
    ~File716SendTask();

    static File716SendTask * instance();

    bool addTransmit(std::shared_ptr<ClientNetwork::BaseTransmit> trans,SendCallbackFunc callback = nullptr);
    bool removaAllTransmit();
    std::shared_ptr<FileSendDesc> removeTask(QString No);

    void startMe();
    void stopMe();

protected:
    void run();

private:
    void prepareSendTask();
    void processFileData();
    unsigned long countPackAllLen(int fileSize,int fileNameLen);

private:
    static File716SendTask * recordTask;

    int maxTransferFiles;                                   /*!< 最大传输的文件数量 */
    std::mutex sendFileMutex;
    std::list<std::shared_ptr<FileSendDesc>> sendList;      /*!< 正在发送的文件信息 */

    std::mutex tranMutex;
    std::map<CommMethod,std::pair<std::shared_ptr<ClientNetwork::BaseTransmit>,SendCallbackFunc>> transmits;
};

#endif

#endif // FILE716SENDTASK_H
