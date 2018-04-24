/*!
 *  @brief     数据解析
 *  @details   响应线程调用，验证数据合法性，解析数据类型
 *  @file      dataprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @attention 服务器根据启动的服务类型，自动选择对应的解析方式， @n
 *             1.文本传输服务：采用字符型解析方式，通过使用JSON来加快解析的速度 @n
 *             2.文件传输服务：采用字节型解析方式，通过RBuffer获取原始的字节流，然后按位解析 @n
 *  @copyright NanJing RenGu.
 */

#ifndef DATAPARSE_H
#define DATAPARSE_H

#include "Network/head.h"

#include <QObject>
#include <QJsonObject>
#include "Util/rbuffer.h"

class Database;


class DataParse : public QObject
{
    Q_OBJECT
public:
    explicit DataParse(QObject *parent = 0);

    void processData(Database *db, const SocketInData & data);

private:
    void parseControlData(Database *db, int socketId, QJsonObject & obj);
    void parseTextData(Database * db,int socketId,QJsonObject &obj);
    void parseFileData(Database * db, int socketId, RBuffer &buffer);

    void onProcessUserRegist(Database * db,int socketId,QJsonObject &obj);
    void onProcessUserLogin(Database * db,int socketId, QJsonObject & obj);
    void onProcessUpdateUserInfo(Database * db,int socketId,QJsonObject &obj);
    void onProcessUserStateChanged(Database * db,int socketId,QJsonObject &obj);

    void onProcessSearchFriend(Database * db,int socketId,QJsonObject &obj);
    void onProcessAddFriend(Database * db,int socketId,QJsonObject &obj);
    void onProcessRelationOperate(Database * db,int socketId,QJsonObject &obj);
    void onProcessFriendListOperate(Database * db,int socketId,QJsonObject &obj);
    void onProcessGroupingFriend(Database * db,int socketId,QJsonObject &obj);

    void onProcessGroupingOperate(Database * db,int socketId,QJsonObject &obj);

    void onProcessGroupList(Database * db,int socketId,QJsonObject &obj);

    void onProcessFileRequest(Database * db, int socketId, RBuffer &obj);
    void onProcessFileControl(Database * db,int socketId,RBuffer &obj);
    void onProcessFileData(Database * db, int socketId, RBuffer &obj);

};

#endif // DATAPROCESS_H
