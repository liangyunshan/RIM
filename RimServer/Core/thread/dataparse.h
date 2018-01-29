/*!
 *  @brief     数据解析
 *  @details   响应线程调用，验证数据合法性，解析数据类型
 *  @file      dataprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef DATAPARSE_H
#define DATAPARSE_H

#include "Network/head.h"

#include <QObject>
#include <QJsonObject>

class Database;


class DataParse : public QObject
{
    Q_OBJECT
public:
    explicit DataParse(QObject *parent = 0);

    void processData(Database *db, const SocketInData & data);

private:
    void parseControlData(Database *db, int socketId, QJsonObject & obj);

    void onProcessUserRegist(Database * db,int socketId,QJsonObject &obj);
    void onProcessUserLogin(Database * db,int socketId, QJsonObject & obj);
    void onProcessUpdateUserInfo(Database * db,int socketId,QJsonObject &obj);

    void onProcessSearchFriend(Database * db,int socketId,QJsonObject &obj);
    void onProcessAddFriend(Database * db,int socketId,QJsonObject &obj);

};

#endif // DATAPROCESS_H
