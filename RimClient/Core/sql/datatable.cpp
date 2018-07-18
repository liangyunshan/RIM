#include "datatable.h"

#include "../global.h"
#include "../user/user.h"
#include "../sql/database.h"

#include <QSqlQuery>

namespace DataTable {

// SQLite中主键自增不能使用int类型，需要使用integer类型
const QString SQLHistoryChat = "CREATE TABLE  IF NOT EXISTS `rhistorychat`("
                             "`ID`  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                             "`TYPE`  tinyint(2) NOT NULL ,"
                             "`ACCOUNTID`  varchar(20) NOT NULL ,"
                             "`NICKNAME`  varchar(50) NULL ,"
                             "`DTIME`  bigint  NOT NULL ,"
                             "`ISTOP` tinyint(1) NOT NULL,"
                             "`LASTRECORD`  varchar(255) NULL ,"
                             "`SYSTEMICON`  tinyint(1) NOT NULL ,"
                             "`ICONID`  varchar(50) NOT NULL"
                             ")";

const QString SQLChatList = "CREATE TABLE  IF NOT EXISTS `rchatlist`("
                          "`ID`  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                          "`ACCOUNTID`  varchar(50) NOT NULL ,"
                          "`FTIME`  int(10) NOT NULL"
                          ")";

const QString SQLChatRecord = "CREATE TABLE  IF NOT EXISTS `rchatrecord`("
                            "`ID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                            "`ACCOUNTID`  varchar(50) NOT NULL ,"
                            "`NICKNAME`  varchar(50) NULL ,"
                            "`TTIME`  int(10) NULL ,"
                            "`TYPE`  tinyint(2) NULL ,"
                            "`DATA`  varchar(255) NULL"
                            ")";


RHistoryChat::RHistoryChat():table("rhistorychat"),id("ID"),type("TYPE"),accountId("ACCOUNTID"),nickName("NICKNAME"),
    dtime("DTIME"),lastRecord("LASTRECORD"),isTop("ISTOP"),systemIon("SYSTEMICON"),iconId("ICONID")
{

}

RChatList::RChatList():table("rchatlist"),id("ID"),accountId("ACCOUNTID"),firstChatTime("FTIME")
{

}

RChatRecord::RChatRecord():table("rchatrecord"),id("ID"),accountId("ACCOUNTID"),nickName("NICKNAME"),time("TTIME"),type("TYPE"),
    dateTime("DATETIME"),serialNo("SERIALNO"),status("STATUS"),data("DATA")
{

}

RChatSerialNo::RChatSerialNo():table("rchatserialno"),id("ID"),serialno("SERIALNO")
{

}


bool RChatRecord::initTable(const QString &name)
{
   table = name;
   QString SQLPersonalChatRecord = QString("CREATE TABLE  IF NOT EXISTS `%1`("
                                           "`ID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                                           "`ACCOUNTID`  varchar(50) NOT NULL ,"
                                           "`NICKNAME`  varchar(50) NULL ,"
                                           "`TTIME`  int(10) NULL ,"
                                           "`TYPE`  tinyint(2) NULL ,"
                                           "`DATETIME`  varchar(7) NOT NULL,"
                                           "`SERIALNO`  tinyint(2) NOT NULL,"
                                           "`STATUS`  tinyint(2) NULL ,"
                                           "`DATA`  varchar(255) NULL"
                                           ")").arg(table);
   QSqlQuery query(G_User->database()->sqlDatabase());
   if(!query.exec(SQLPersonalChatRecord))
       return false;

   return true;
}

bool RChatSerialNo::initTable(const QString &name)
{
    table = name;
    QString SQLSerialNo = QString("CREATE TABLE  IF NOT EXISTS `%1`("
                                            "`ID` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
                                            "`SERIALNO`  int(10) NOT NULL"
                                            ")").arg(table);
    QSqlQuery query(G_User->database()->sqlDatabase());
    if(!query.exec(SQLSerialNo))
        return false;

    return true;
}

}


