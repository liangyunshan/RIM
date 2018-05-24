#include "raes.h"
#include "AES.h"

#include <QByteArray>
#include <QDebug>

RAES::RAES()
{
    unsigned char key[] =
    {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };

    m_aes = NULL;
    m_aes = new AES(key);
}

RAES::~RAES()
{
    if(m_aes)
    {
        m_aes = NULL;
        delete m_aes;
    }
}

//加密
QByteArray RAES::aesData(QByteArray &data)
{
    //[1]AES加密
    unsigned char* str = NULL;
    str =(unsigned char*)qstrdup(data.constData());

    //预留够16整数倍字节的空间
    qint64 length = data.length();
    qint64 useLength = length;
    if(length%16 != 0)
    {
        int area = length/16;
        useLength = (area+1)*16;
    }

    m_aes->Cipher((void *)str,useLength);
    QByteArray aesbuff((char*)str,useLength);
    int totleSize = aesbuff.size()+sizeof(qint64);
    char* write_str = new char[totleSize];
    memset(write_str,0,totleSize);
    memcpy(write_str,&length,sizeof(qint64));
    memcpy(write_str+sizeof(qint64),str,aesbuff.size());

    QByteArray newbuff(write_str,totleSize);
    //[~1]

    return newbuff;
}

//解密
QByteArray RAES::unAesData(QByteArray &data)
{
    qint64 length = data.size();

    char *srcStr = data.data();
    qint64 reallength = 0;
    memcpy(&reallength,srcStr,sizeof(qint64));

    //预留够16整数倍字节的空间
    qint64 useLength = length;
    if(length%16 != 0)
    {
        int area = length/16;
        useLength = (area+1)*16;
    }

    char *str = new char[useLength];
    memset(str,0,useLength);
    memcpy(str,srcStr+sizeof(qint64),length-sizeof(qint64));

    m_aes->InvCipher((void *)str,useLength);

    QByteArray unaesbuff(str,reallength);
    return unaesbuff;
}
