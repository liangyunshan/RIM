#include "serialno.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QReadWriteLock>
#include <QWriteLocker>
#include "../sql/database.h"
#include "../sql/datatable.h"
#include "../sql/rpersistence.h"
#include "../global.h"
#include "../user/user.h"

SerialNo * SerialNo::staic_SerialNo = NULL ;
const unsigned short SERIALNO_MAX   = 65535 ;

QReadWriteLock SerialNo_lock(QReadWriteLock::Recursive);

SerialNo::SerialNo()
{
    m_SerialNo = 1;
}

SerialNo *SerialNo::instance()
{
    if(staic_SerialNo == NULL)
    {
        staic_SerialNo = new SerialNo();
    }
    return staic_SerialNo;
}

unsigned short SerialNo::FrashSerialNo()
{
    QWriteLocker locker(&SerialNo_lock);
    m_SerialNo>=SERIALNO_MAX?(m_SerialNo=1) : (m_SerialNo+=1);
    return m_SerialNo;
}

void SerialNo::SetSerialNo(unsigned short No)
{
    QWriteLocker locker(&SerialNo_lock);
    m_SerialNo = No;
}

bool SerialNo::updateSqlSerialNo( unsigned short No)
{
    DataTable::RChatSerialNo rhc;
    rhc.initTable("rchatserialno");
    RUpdate rpd(rhc.table);
    rpd.update(rhc.table,{{rhc.serialno,No}})
            .enableAlias(false)
            .createCriteria()
            .add(Restrictions::eq(rhc.table,rhc.id,1));

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rpd.sql()))
    {
        return true;
    }

    return false;
}

unsigned short SerialNo::getSqlSerialNo()
{
    DataTable::RChatSerialNo rhc;
    rhc.initTable("rchatserialno");
    RSelect rst(rhc.table);
    rst.select(rhc.table,{{rhc.id},
                          {rhc.serialno}})
            .createCriteria()
            .add(Restrictions::eq(rhc.table,rhc.id,1));

    QSqlQuery query(G_User->database()->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            return query.value(rhc.serialno).toString().toUShort();
        }

        RPersistence rps(rhc.table);
        rps.insert({{rhc.id,1},
                    {rhc.serialno,1}});
        if(query.exec(rps.sql()))
        {
            return 1;
        }
    }

    return 0;
}
