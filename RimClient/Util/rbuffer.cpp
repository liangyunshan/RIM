#include "rbuffer.h"

#include <QDebug>
#include <qglobal.h>

RBuffer::RBuffer(int defaultSize):pHead(0),dataSize(0),
    buffSize(defaultSize)
{
    dataBuff = new char[defaultSize];
    memset(dataBuff,0,sizeof(defaultSize));
}

RBuffer::RBuffer(QByteArray array):buffSize(BUFFER_DEFAULT_SIZE),
    pHead(0)
{
    int size = array.size()+1 > buffSize ?array.size()+1:buffSize;
    dataSize = array.size();
    dataBuff = new char[size];
    memset(dataBuff,0,size);
    memcpy(dataBuff,array.data(),array.size());
}

RBuffer::~RBuffer()
{
    delete[] dataBuff;
}

bool RBuffer::append(int value)
{
    if((pHead + sizeof(int)) >=  maxSize())
        return false;
    memcpy(dataBuff + pHead,(char *)&value,sizeof(int));
    pHead+= sizeof(int);

    return true;
}

bool RBuffer::append(size_t value)
{
    if((pHead + sizeof(size_t)) >=  maxSize())
        return false;
    memcpy(dataBuff + pHead,(char *)&value,sizeof(size_t));
    pHead+= sizeof(size_t);

    return true;
}

bool RBuffer::append(double value)
{
    if((pHead + sizeof(double)) >=  maxSize())
        return false;
    memcpy(dataBuff + pHead,(char *)&value,sizeof(double));
    pHead+= sizeof(double);
    return true;
}

/*!
 * @warning 在写入字符串时，需要先用4字节写入字符串的长度，在解析时需要注意。
 */
bool RBuffer::append(QString &string)
{
    int stringSize = string.toLocal8Bit().size();
    if((pHead + stringSize + sizeof(int)) >=  maxSize())
        return false;

    compressStringLen(string.toLocal8Bit().size());

    memcpy(dataBuff + pHead,(char *)string.toLocal8Bit().data(),stringSize);
    pHead+= stringSize;
    return true;
}

bool RBuffer::append(const char *data, int len)
{
    if((pHead + len)>=  maxSize() || data == NULL)
        return false;

    compressStringLen(len);

    memcpy(dataBuff + pHead,data,len);
    pHead+= len;
    return true;
}

bool RBuffer::read(int &value)
{
    if(pHead + sizeof(int) > size())
        return false;
    memcpy((char *)&value,dataBuff+pHead,sizeof(int));
    pHead += sizeof(int);
    return true;
}

bool RBuffer::read(size_t &value)
{
    if(pHead + sizeof(size_t) > size())
        return false;
    memcpy((char *)&value,dataBuff+pHead,sizeof(size_t));
    pHead += sizeof(size_t);
    return true;
}

bool RBuffer::read(double &value)
{
    if(pHead + sizeof(double) > size())
        return false;
    memcpy((char *)&value,dataBuff+pHead,sizeof(double));
    pHead += sizeof(double);
    return true;
}

/*!
 * @brief 向窗口的工具栏中插入工具按钮，默认是自左向右排列
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
bool RBuffer::read(const char **buff, int maxLen,size_t & outLen)
{
    outLen = 0;
    size_t uncompressLen;
    size_t filedLen;
    //[1]解析字符串数据长度
    if(!readLengthWithoutOffset(uncompressLen,filedLen))
        return false;
    //[2]
    if(maxLen != 0 && filedLen > maxLen)
        return false;

    outLen = filedLen;

    if(pHead + outLen > size())
        return false;

    pHead += uncompressLen;

    *buff = (dataBuff + pHead);

    pHead += filedLen;

    return true;
}

bool RBuffer::read(QString &string)
{
    const char * data = NULL;
    size_t dataLen = 0;
    if(!read(&data,BUFFER_DEFAULT_SIZE,dataLen))
        return false;
    string.append(QByteArray(data,dataLen));

    return true;
}

/*!
 * @brief 跳过对应字符的长度
 * @attention 用于容器作为读操作时
 * @param[in] 无
 * @return 返回是否跳过成功
 */
bool RBuffer::skipInt()
{
    if(pHead + sizeof(int) > size())
        return false;
    pHead += sizeof(int);
    return true;
}

bool RBuffer::skipDouble()
{
    if(pHead + sizeof(double) > size())
        return false;
    pHead += sizeof(double);
    return true;
}

/*!
 * @brief 移动读指针
 * @param[in] pos 待移动的位置
 * @return 无
 */
bool RBuffer::seek(int pos)
{
    if(pos < 0 || pos > size())
        return false;

    pHead = pos;
    return true;
}

int RBuffer::pos() const
{
    return pHead;
}

/*!
 * @brief 获取数据长度
 * @return 返回缓冲区数据长度
 */
size_t RBuffer::size()
{
   dataSize = pHead > dataSize ? pHead:dataSize;
   return dataSize;
}

/*!
 * @brief 获取容器最大长度
 * @return 返回容器长度
 */
size_t RBuffer::maxSize() const
{
    return buffSize;
}

void RBuffer::compressStringLen(int length)
{
    char stringLen[5]={0};
    unsigned int compressLen = 0;
    compress(length,stringLen,compressLen);
    memcpy(dataBuff + pHead,stringLen,compressLen);
    pHead += compressLen;
}

/*!
 * @brief 对整形进行压缩成八进制
 * @details 将整形数据压缩成八进制，减少内存的占用
 * @param[in] i 待压缩的数字
 * @param[in] buf 压缩后的缓冲区
 * @param[in] len 压缩后的长度
 * @return 返回是否压缩成功
 */
bool RBuffer::compress(unsigned int i, char *buf, unsigned int &len)
{
    len = 0;
    for (int a = 4; a >= 0; a--)
    {
        char c;
        c = i >> (a * 7) & 0x7f;
        if (c == 0x00 && len == 0)
            continue;

        if (a == 0)
            c &= 0x7f;
        else
            c |= 0x80;
        buf[len] = c;
        len++;
    }
    if (len == 0)
    {
        len++;
        buf[0] = 0;
    }
    return true;
}

bool RBuffer::uncompress(char *buf, size_t len, unsigned int &i)
{
    i = 0;
    for (int index = 0; index < (int)len; index++)
    {
        char c = *(buf + index);
        i = i << 7;

        c &= 0x7f;
        i |= c;
    }
    return true;
}

/*!
 * @brief 反向解析字符串长度
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
bool RBuffer::readLengthWithoutOffset(size_t& headlen, size_t & outlen)
{
    headlen = 0;
    const char * cur = dataBuff+pHead;
    const char *temp = cur;
    char buf[5];
    for (size_t i = 0; i<sizeof(buf); i++)
    {
        memcpy(buf + i, temp, sizeof(char));
        temp++;
        headlen++;

        if ((buf[i] & 0x80) == 0x00)
            break;
    }
    if (cur + headlen > dataBuff + size())
        return false;

    unsigned int value;
    uncompress(buf, headlen, value);
    outlen = value;
    return true;
}

const char *RBuffer::data()
{
    return dataBuff;
}

QByteArray RBuffer::byteArray()
{
    QByteArray array;
    array.append(dataBuff,size());
    return array;
}
