/*!
 *  @brief     数据执行处理
 *  @details   用于对数据库进行操作
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.01.30
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SQLPROCESS_H
#define SQLPROCESS_H

#include <QObject>
#include "datastruct.h"
#include "sqlheader.h"

using namespace SQLHeader;

class Database;

class SQLProcess
{
public:
    SQLProcess();
    ~SQLProcess();
    static SQLProcess *instance();


    QString querryRecords(int userid, int currRow=0, int queryRows= TextUnit::DefaultQueryRow);
    int queryTotleRecord(Database * db,int id);

    bool createTablebUserList(Database * db);
    bool queryUser(Database * db, int tgtUserId);
    bool insertTgtUser(Database * db, int tgtUserId, QString name);
    bool createTableUser_id(Database * db, int tgtUserId);
    bool insertTableUserChatInfo(Database * db, TextUnit::ChatInfoUnit unit);

private:
    unsigned short m_port;
    unsigned short m_QueryRow;
};

#endif // SQLPROCESS_H
