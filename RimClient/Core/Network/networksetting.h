/*!
 *  @brief     网络参数结构
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef NETWORKSETTING_H
#define NETWORKSETTING_H

#include <QString>
#include <QVector>

struct IpPort
{
    IpPort();
    IpPort(QString ip,unsigned short port);
    IpPort(const IpPort & otherSide);

    IpPort operator=(const IpPort & otherSide);

    bool operator== (const IpPort & otherSide);

    bool isValid();
    void setConnected(bool flag){connected = flag;}
    bool isConnected(){return connected;}

    QString ip;
    unsigned short port;

    bool connected;
};

/*!
 *  @brief 网络配置信息
 */
struct NetworkSettings{

    QVector<IpPort> validTextIps();

#ifndef __LOCAL_CONTACT__
    QVector<IpPort> validFileIps();
#endif

    IpPort textServer;              //主服务器
#ifdef __LOCAL_CONTACT__
    IpPort tandemServer;            //串联服务器
#else
    IpPort fileServer;              //文件服务器
#endif

    IpPort connectedTextIpPort;         //保存正在连接的文本网络地址信息(要么为textServer，要么为tandemServer)
#ifndef __LOCAL_CONTACT__
    IpPort connectedFileIpPort;         //保存正在连接的文件网络地址信息
#endif
};

#endif // NETWORKSETTING_H
