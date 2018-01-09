#include "subject.h"

#include <QMutex>
#include <QReadWriteLock>
#include <QWriteLocker>

#include "head.h"
#include "observer.h"

QReadWriteLock lock;

Subject::Subject()
{

}

void Subject::attach(Observer *obj)
{
    QWriteLocker locker(&lock);
    observers.append(obj);

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<observers.size()<<"__"<<obj;
}

void Subject::detach(Observer *obj)
{
    QWriteLocker locker(&lock);
    observers.removeOne(obj);
    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<observers.size()<<"__"<<obj;
}

void Subject::notify(MessageType type)
{
    QWriteLocker locker(&lock);
    QList<Observer *>::iterator iter = observers.begin();
    while(iter != observers.end())
    {
        (*iter)->onMessage(type);
        iter++;
    }
}
