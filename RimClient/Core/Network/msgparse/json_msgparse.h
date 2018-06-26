/*!
 *  @brief     JSON格式解析
 *  @details   默认采用JSON作为数据解析的格式
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.25
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef JSON_MSGPARSE_H
#define JSON_MSGPARSE_H

#include <QJsonObject>
#include <QJsonParseError>

#include "dataparse.h"
#include "Util/rbuffer.h"

#include "../../protocol/protocoldata.h"
using namespace ProtocolType;

class Json_MsgParse : public DataParse
{
public:
    Json_MsgParse();

    void processData(const QByteArray &recvData);

private:
    void handleCommandMsg(MsgCommand commandType, QJsonObject &obj);
    void handleTextMsg(MsgCommand commandType, QJsonObject &obj);

private:
    QJsonParseError jsonParseError;

};

#endif // JSON_MSGPARSE_H
