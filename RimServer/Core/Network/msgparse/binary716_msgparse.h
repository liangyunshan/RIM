/*!
 *  @brief     716通信数据解析
 *  @details   用于将接收的716原始数据进行解析处理
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BINARY716_MSGPARSE_H
#define BINARY716_MSGPARSE_H

#include "dataparse.h"

#ifdef __LOCAL_CONTACT__

class Binary716_MsgParse : public DataParse
{
public:
    explicit Binary716_MsgParse();

    void processData(Database *db, const RecvUnit & unit);
};

#endif

#endif // BINARY716_MSGPARSE_H
