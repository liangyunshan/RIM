/*!
 *  @brief     接收文件描述
 *  @details   用于描述接收的文件信息
 *  @file      filedesc.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.20
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef FILERECVDESC_H
#define FILERECVDESC_H

#include <QString>

class QFile;

class FileRecvDesc
{
public:
    explicit FileRecvDesc();
     ~FileRecvDesc();

    void setFilePath(const QString filePath);

    bool create();
    bool isNull(){return !file;}
    bool seek(size_t pos);
    bool flush();
    bool isRecvOver();

    qint64 write(const QByteArray &data);
    qint64 getWriteSize(){return writeLen;}
    qint64 fileSize(){return size;}

    void close();
    void destory();

private:
    int itemType;                        /*!< 文件操作类型 @link FileItemType @endlink */
    qint64 size;                         /*!< 文件大小 */
    qint64 writeLen;                     /*!< 文件已经写入的大小 */
    QString fileName;                    /*!< 文件名称 @attention 维护文件真实的信息 */
    QString md5;                         /*!< 文件MD5 */
    QString accountId;                   /*!< 自己ID */
    QString otherId;                     /*!< 接收方ID */
    QString saveFilePath;                /*!< 保存文件的路径，默认保存至用户文件夹的接收目录下，可修改 */
    QFile * file;                        /*!< 文件缓冲 */

    friend class FileManager;
};

#endif // FILEDESC_H
