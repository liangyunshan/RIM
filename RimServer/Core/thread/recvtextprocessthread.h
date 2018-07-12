/*!
 *  @brief     接收聊天处理线程
 *  @details   处理socket接收到的信息，根据信息的类型依次处理；
 *  @file      recvtextprocessthread.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RECVTEXTPROCESSTHREAD_H
#define RECVTEXTPROCESSTHREAD_H

#include <QThread>

#include "sql/database.h"

class RecvTextProcessThread : public QThread
{
    Q_OBJECT
public:
    explicit RecvTextProcessThread();

    void setDatabase(Database * db);

protected:
    void run();

private:
    Database * database;
    bool runningFlag;
};

#endif // RECVTEXTPROCESSTHREAD_H
