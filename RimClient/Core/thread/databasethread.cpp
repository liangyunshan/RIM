#include "databasethread.h"

#include "sql/database.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QDebug>

DatabaseThread::DatabaseThread(QObject *obj):
    p_TaskObj(obj)
{
    database = NULL;
}

void DatabaseThread::setDatabase(Database *db)
{
    database = db;
}

void DatabaseThread::addSqlQueryTask(int id, QString sql_querry)
{
    TaskQueue task;
    task.id = id;
    task.cmd = sql_querry;
    m_TaskQueue.enqueue(task);
    if(!isRunning())
    {
        start();
    }
    else
    {
        runWaitCondition.wakeOne();
    }
}

void DatabaseThread::run()
{
    while(true)
    {
        while (!m_TaskQueue.isEmpty())
        {
            TaskQueue task = m_TaskQueue.head();
            if(database)
            {
                TextUnit::ChatInfoUnitList list;

                QSqlQuery query(database->sqlDatabase());
                query.prepare(task.cmd);
                bool ret = query.exec();
                if(ret)
                {
                    while(query.next())
                    {
                        TextUnit::ChatInfoUnit unit;
                        unit.user.id    = query.value(TextUnit::_Sql_User_UserId_).toInt();
                        unit.user.name  = query.value(TextUnit::_Sql_User_UserName_).toString();
                        unit.user.head  = query.value(TextUnit::_Sql_User_Userhead_).toString();
                        unit.time       = query.value(TextUnit::_Sql_User_RecordTime_).toString();
                        unit.contents   = query.value(TextUnit::_Sql_User_RecordContents_).toString();
                        list.append(unit);
                    }
                    emit resultReady(0,list);
                }
                else
                {
                    emit resultReady(-1,list);
                }
            }
            m_TaskQueue.removeFirst();
        }

        if(m_TaskQueue.isEmpty())
        {
            m_Pause.lock();
                runWaitCondition.wait(&m_Pause);
            m_Pause.unlock();
        }

        qDebug()<<__FILE__<<__LINE__<<"\n"
               <<"this->m_Pause:"<<m_Pause.isRecursive()
              <<"\n";
    }
}
