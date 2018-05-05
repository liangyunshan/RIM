#ifndef DATABASETHREAD_H
#define DATABASETHREAD_H

#include <QQueue>
#include <QThread>
#include "sql/database.h"
#include "datastruct.h"
#include <QWaitCondition>

class DatabaseThread : public QThread
{
    Q_OBJECT
public:
    struct TaskQueue{
        int id;
        QString cmd;

        TaskQueue(){
            id = 0;
            cmd = "";
        }
    };
    explicit DatabaseThread(QObject *obj=NULL);
    ~DatabaseThread();

    void setDatabase(Database * db);
    void addSqlQueryTask(int id, QString sql_querry);

protected:
    virtual void run() Q_DECL_OVERRIDE ;

private:
    Database * database;
    QObject *p_TaskObj;
    QQueue<TaskQueue> m_TaskQueue;
    QWaitCondition runWaitCondition;
    QMutex m_Pause;
    bool runningFlag;

signals:
    void resultReady(int ,TextUnit::ChatInfoUnitList);

};

#endif // DATABASETHREAD_H
