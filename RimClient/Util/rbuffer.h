/*!
 *  @brief     自定义缓冲区
 *  @details   在Qt5.7中，使用QBuffer和QDataStream时，容易报ASSERT: "bytes <= bufferSize" in file tools\qringbuffer.cpp 的错误，
 *             暂未找到解决方法，遂通过自定义的缓冲区，为制作协议时提供一点帮助。
 *  @file      rbuffer.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RBUFFER_H
#define RBUFFER_H

#include <QByteArray>
#include <QString>
#include "util_global.h"

#define BUFFER_DEFAULT_SIZE 1024

class UTILSHARED_EXPORT RBuffer
{
public:
    RBuffer(int defaultSize = BUFFER_DEFAULT_SIZE);
    ~RBuffer();

    RBuffer(QByteArray  array);

    bool append(int value);
    bool append(size_t value);
    bool append(double value);
    bool append(QString &string);
    bool append(const char * data,int len);

    bool read(int & value);
    bool read(size_t & value);
    bool read(double &value);
    bool read(const char **buff, int maxLen, size_t &outLen);
    bool read(QString &string);

    bool skipInt();
    bool skipDouble();

    bool seek(int pos = 0);
    int pos()const;
    size_t size();
    size_t maxSize()const;

    const char * data();
    QByteArray byteArray();

private:
    void compressStringLen(int length);
    bool compress(unsigned int i, char *buf, unsigned int &len);
    bool uncompress(char *buf, size_t len, unsigned int &i);
    bool readLengthWithoutOffset(size_t& headlen, size_t & outlen);

private:
    char * dataBuff;           /*!< 数据缓冲区索引地址 */
    size_t buffSize;           /*!< 缓冲区的总长度 */
    size_t dataSize;           /*!< 数据长度，数据长度小于等于缓冲区总长度 */
    size_t pHead;              /*!< 头指针，指向数据可写区域 */
};

#endif // RBUFFER_H
