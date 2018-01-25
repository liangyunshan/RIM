#include "sqlprocess.h"

#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QMutex>
#include <QDate>

#include "datatable.h"
#include "sql/database.h"
#include "Util/rutil.h"
#include "constants.h"
#include "rpersistence.h"

QMutex ACCOUNT_LOCK;

int G_BaseAccountId = 0;

SQLProcess::SQLProcess()
{

}

ResponseRegister SQLProcess::processUserRegist(Database *db, const RegistRequest *request, QString &id)
{
    DataTable::RimConfig config;
    bool sqlError = true;
    ACCOUNT_LOCK.lock();

    if(G_BaseAccountId == 0)
    {
        RSelect rst(config.table);
        rst.select(config.value);
        rst.addCondition(config.name,config.accuoutId);

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()))
        {
            if(query.next())
            {
               G_BaseAccountId = query.value(0).toInt();
               sqlError = false;
            }
        }
    }
    else
    {
        sqlError = false;
    }

    G_BaseAccountId++;

    ACCOUNT_LOCK.unlock();

    if(sqlError)
    {
        return REGISTER_FAILED;
    }

    DataTable::User user;
    RPersistence rpc(user.table);
    rpc.insert(user.id,RUtil::UUID());
    rpc.insert(user.account,QString::number(G_BaseAccountId));
    rpc.insert(user.password,request->password);
    rpc.insert(user.nickName,request->nickName);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpc.sql()))
    {
        id  = QString::number(G_BaseAccountId);

        RUpdate rpd(config.table);
        rpd.update(config.value,G_BaseAccountId);
        rpd.addCondition(config.name,config.accuoutId);

        if(query.exec(rpd.sql()))
        {

        }

        return REGISTER_SUCCESS;
    }

    return REGISTER_FAILED;
}

ResponseLogin SQLProcess::processUserLogin(Database * db,const LoginRequest *request)
{
    DataTable::User user;

    RSelect rst(user.table);
    rst.select(user.password);
    rst.addCondition(user.account,request->accountId);

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(rst.sql()))
    {
        if(query.next())
        {
             if(query.value(0).toString() != request->password)
             {
                    return LOGIN_PASS_ERROR;
             }
             else
             {
                    return LOGIN_SUCCESS;
             }
        }
        else
        {
            return LOGIN_UNREGISTERED;
        }
    }

    return LOGIN_FAILED;
}

ResponseUpdateUser SQLProcess::processUpdateUserInfo(Database *db, const UpdateBaseInfoRequest *request)
{
    DataTable::User user;

    RUpdate rpd(user.table);
    rpd.update(user.nickName,request->baseInfo.nickName);
    rpd.update(user.signName,request->baseInfo.signName);
    rpd.update(user.gender,(int)request->baseInfo.sexual);
    rpd.update(user.birthDay,QDate::fromString(request->baseInfo.birthday,Constant::Date_Simple));
    rpd.update(user.phone,request->baseInfo.phoneNumber);
    rpd.update(user.address,request->baseInfo.address);
    rpd.update(user.email,request->baseInfo.email);
    rpd.update(user.remark,request->baseInfo.remark);
    rpd.update(user.face,request->baseInfo.face);
    rpd.update(user.faceId,request->baseInfo.customImgId);

    rpd.addCondition(user.account,request->baseInfo.accountId);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpd.sql()))
    {
        return UPDATE_USER_SUCCESS;
    }

    return UPDATE_USER_FAILED;
}

void SQLProcess::getUserInfo(Database *db,const QString accountId, UserBaseInfo & userInfo)
{
    DataTable::User user;

    RSelect rst(user.table);
    rst.addCondition(user.account ,accountId);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        if(query.next())
        {
            userInfo.accountId = accountId;
            userInfo.nickName = query.value(user.nickName).toString();
            userInfo.signName = query.value(user.signName).toString();
            userInfo.sexual = (Sexual)query.value(user.gender).toInt();
            userInfo.birthday = query.value(user.birthDay).toDate().toString(Constant::Date_Simple);
            userInfo.address = query.value(user.address).toString();
            userInfo.email = query.value(user.email).toString();
            userInfo.phoneNumber = query.value(user.phone).toString();
            userInfo.remark = query.value(user.remark).toString();
            userInfo.face = query.value(user.face).toInt();
            userInfo.customImgId = query.value(user.faceId).toString();
        }
    }
}

