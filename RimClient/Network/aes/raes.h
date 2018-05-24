/*!
 *  @brief     AES信息加密解密算法类
 *  @details   封装处理了数据的加密和解密使用接口，Qt实现
 *  @file      raes.h
 *  @author    shangchao
 *  @version   1.0
 *  @date      2018.05.24
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef RAES_H
#define RAES_H

#include <QObject>
class AES;

class RAES
{
public:
    RAES();
    ~RAES();

    QByteArray aesData(QByteArray & data);

    QByteArray unAesData(QByteArray & data);

private:
    AES *m_aes;
};

#endif // RAES_H
