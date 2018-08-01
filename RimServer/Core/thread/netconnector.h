/*!
 *  @brief     网络连接器
 *  @details   处理本服务器连接远程服务器的请求，若建立请求，则将连接加入SeriesConnection队列中统一管理 \n
 *             可能存在同一时刻向多个服务器进行网络连接请求，因此每个请求均有单独的连接请求线程管理和维护
 *  @author    wey
 *  @version   1.0
 *  @date      2018.07.03
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef NETCONNECTOR_H
#define NETCONNECTOR_H

#include <QThread>
#include <functional>

#include "sql/database.h"
#include "../protocol/datastruct.h"
#include "../Network/head.h"

class NetConnector;
typedef std::function<void(unsigned short,bool,int)> NetFunc;

#ifdef __LOCAL_CONTACT__
void RunNetConnetor(NetFunc func,ParameterSettings::NodeServer & server);

class NetConnector : public QThread
{
    Q_OBJECT
public:
    explicit NetConnector(QObject * parent = 0);
    ~NetConnector();

    void bindServer(ParameterSettings::NodeServer & server);

    void registCallBack(NetFunc func);

protected:
    void run();

private:
    char buff[SOCK_CHAR_BUFF_LEN];
    unsigned short port;
    bool dataReady;
    unsigned short nodeId;

    NetFunc nfc;
};

#endif

#endif // NETCONNECTOR_H
