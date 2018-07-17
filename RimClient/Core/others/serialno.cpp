#include "serialno.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include "../sql/database.h"
#include "../sql/datatable.h"
#include "../sql/rpersistence.h"
#include "../global.h"
#include "../user/user.h"

#include <QDebug>

#include <mutex>
#include <condition_variable>

SerialNo * SerialNo::staic_SerialNo = NULL;

static unsigned short SERIALNO_STATIC = 1;
static unsigned short SERIALNO_MAX_STATIC = 65535;

std::mutex SerialNoMutex;

SerialNo::SerialNo()
{

}

SerialNo *SerialNo::instance()
{
    if(staic_SerialNo == NULL)
    {
        staic_SerialNo = new SerialNo();
    }
    return staic_SerialNo;
}

unsigned int SerialNo::FrashSerialNo()
{
    SerialNoMutex.lock();
    SERIALNO_STATIC>=SERIALNO_MAX_STATIC?(SERIALNO_STATIC=1) : (SERIALNO_STATIC+=1);
    SerialNoMutex.unlock();
    return SERIALNO_STATIC;
}

unsigned int SerialNo::SetSerialNo(unsigned int No)
{
    SerialNoMutex.lock();
    SERIALNO_STATIC = No;
    SerialNoMutex.unlock();
    return SERIALNO_STATIC;
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
