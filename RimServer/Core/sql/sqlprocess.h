/*!
 *  @brief     数据执行处理
 *  @details   用于对数据库进行操作
 *  @file      sqlprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SQLPROCESS_H
#define SQLPROCESS_H


#include "protocoldata.h"
using namespace ProtocolType;

class Database;

class SQLProcess
{
public:
    SQLProcess();

    bool processUserLogin(Database *db, LoginRequest * request);


private:

};

#endif // SQLPROCESS_H
