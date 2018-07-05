/*!
 *  @brief     处理716数据请求
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATA716PROCESS_H
#define DATA716PROCESS_H

#ifdef __LOCAL_CONTACT__
class Database;

#include "Network/head.h"

class Data716Process
{
public:
    Data716Process();

    void processText(Database * db,int sockId,ProtocolPackage & data);
    void processUserRegist(Database * db,int sockId,ProtocolPackage & data);

};

#endif

#endif // DATA716PROCESS_H
