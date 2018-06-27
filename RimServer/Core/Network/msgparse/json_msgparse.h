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

#include "dataparse.h"
#include "Util/rbuffer.h"

class Json_MsgParse : public DataParse
{
public:
    Json_MsgParse();

    void processData(Database *db, const RecvUnit &data);

private:
    void parseControlData(Database *db, int socketId, QJsonObject & obj);
    void parseTextData(Database * db,int socketId,QJsonObject &obj);
    void parseFileData(Database * db, int socketId, RBuffer &buffer);

    void onProcessUserRegist(Database * db,int socketId,QJsonObject &obj);
    void onProcessUserLogin(Database * db,int socketId, QJsonObject & obj);
    void onProcessUpdateUserInfo(Database * db,int socketId,QJsonObject &obj);
    void onProcessUserStateChanged(Database * db,int socketId,QJsonObject &obj);
    void onProcessHistoryMsg(Database * db,int socketId,QJsonObject &obj);

    void onProcessSearchFriend(Database * db,int socketId,QJsonObject &obj);
    void onProcessAddFriend(Database * db,int socketId,QJsonObject &obj);
    void onProcessRelationOperate(Database * db,int socketId,QJsonObject &obj);
    void onProcessFriendListOperate(Database * db,int socketId,QJsonObject &obj);
    void onProcessGroupingFriend(Database * db,int socketId,QJsonObject &obj);

    void onProcessGroupingOperate(Database * db,int socketId,QJsonObject &obj);

    void onProcessGroupList(Database * db,int socketId,QJsonObject &obj);
    void onProcessRegistGroup(Database * db,int socketId,QJsonObject &obj);
    void onProcessGroupCommand(Database * db,int socketId,QJsonObject &obj);

    void onProcessFileRequest(Database * db, int socketId, RBuffer &obj);
    void onProcessFileControl(Database * db,int socketId,RBuffer &obj);
    void onProcessFileData(Database * db, int socketId, RBuffer &obj);
};

#endif // JSON_MSGPARSE_H
