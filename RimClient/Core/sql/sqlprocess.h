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
#include "../protocol/datastruct.h"
#include "sqlheader.h"
#include "../protocol/protocoldata.h"

using namespace SQLHeader;

class Database;

class SQLProcess
{
public:
    SQLProcess();
    ~SQLProcess();
    static SQLProcess *instance();

    bool createTableIfNotExists(Database * db);

    bool loadChatHistoryChat(Database * db,QList<HistoryChatRecord> & list);
    bool addOneHistoryRecord(Database * db,const HistoryChatRecord & record);
    bool updateOneHistoryRecord(Database * db,const HistoryChatRecord & record);
    bool containHistoryRecord(Database * db,const QString recordId);
    bool removeOneHistoryRecord(Database * db,QString recordId);
    bool removeAllHistoryRecord(Database * db);
    bool topHistoryRecord(Database * db,QString recordId,bool isTop);

private:
    unsigned short m_port;
    unsigned short m_QueryRow;
};

#endif // SQLPROCESS_H
