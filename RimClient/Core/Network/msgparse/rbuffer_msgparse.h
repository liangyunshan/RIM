/*!
 *  @brief     RBuffer数据格式解析
 *  @details
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.26
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RBUFFER_MSGPARSE_H
#define RBUFFER_MSGPARSE_H

#include "dataparse.h"
#include "Util/rbuffer.h"

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

class RBuffer_MsgParse : public DataParse
{
public:
    RBuffer_MsgParse();

    void processData(const QByteArray & recvData);

private:
    void handleFileMsg(MsgCommand commandType, RBuffer &obj);

};

#endif // RBUFFER_MSGPARSE_H
