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
#include "Util/rlog.h"

QMutex ACCOUNT_LOCK;

int G_BaseAccountId = 0;

#define SQL_ERROR -1

SQLProcess::SQLProcess()
{

}

ResponseRegister SQLProcess::processUserRegist(Database *db, const RegistRequest *request, QString &id,QString & uuid)
{
    DataTable::RimConfig config;
    bool sqlError = true;
    ACCOUNT_LOCK.lock();

    if(G_BaseAccountId == 0)
    {
        RSelect rst(config.table);
        rst.select(config.value);
        rst.createCriteria().add(Restrictions::eq(config.name,config.accuoutId));

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

    DataTable::RUser user;
    RPersistence rpc(user.table);
    uuid = RUtil::UUID();
    rpc.insert(user.id,uuid);
    rpc.insert(user.account,QString::number(G_BaseAccountId));
    rpc.insert(user.password,request->password);
    rpc.insert(user.nickName,request->nickName);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpc.sql()))
    {
        id  = QString::number(G_BaseAccountId);

        RUpdate rpd(config.table);
        rpd.update(config.value,G_BaseAccountId);
        rpd.createCriteria().add(Restrictions::eq(config.name,config.accuoutId));

        if(query.exec(rpd.sql()))
        {

        }

        return REGISTER_SUCCESS;
    }

    return REGISTER_FAILED;
}

ResponseLogin SQLProcess::processUserLogin(Database * db,const LoginRequest *request)
{
    DataTable::RUser user;

    RSelect rst(user.table);
    rst.select(user.password);
    rst.createCriteria().add(Restrictions::eq(user.account,request->accountId));

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
    DataTable::RUser user;

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

    rpd.createCriteria().add(Restrictions::eq(user.account,request->baseInfo.accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpd.sql()))
    {
         return UPDATE_USER_SUCCESS;
    }

    return UPDATE_USER_FAILED;
}

ResponseAddFriend SQLProcess::processSearchFriend(Database *db, SearchFriendRequest *request, SearchFriendResponse *response)
{
    DataTable::RUser user;
    DataTable::RChatRoom room;

    QString sql;
    if(request->stype == SearchPerson)
    {
        RSelect rst(user.table);

        rst.select(user.account);
        rst.select(user.nickName);
        rst.select(user.signName);
        rst.select(user.face);
        rst.select(user.faceId);
        rst.createCriteria().add(Restrictions::eq(user.account,request->accountOrNickName))
                .orr(Restrictions::like(user.nickName,request->accountOrNickName));

        sql = rst.sql();
    }
    else
    {
        RSelect rst(room.table);
    }

    QSqlQuery query(db->sqlDatabase());

    if(query.exec(sql))
    {
        if(query.numRowsAffected() > 0)
        {
            while(query.next())
            {
                SimpleUserInfo result;
                result.accountId = query.value(user.account).toString();
                result.nickName = query.value(user.nickName).toString();
                result.signName = query.value(user.signName).toString();
                result.face = query.value(user.face).toInt();
                result.customImgId = query.value(user.faceId).toString();
                response->result.append(result);
            }
            return FIND_FRIEND_FOUND;
        }
        else
        {
            return FIND_FRIEND_NOT_FOUND;
        }
    }
    return FIND_FRIEND_FAILED;
}

ResponseAddFriend SQLProcess::processAddFriendRequest(Database *db,QString accountId,QString operateId,int type)
{
    DataTable::RequestCache rc;

    RPersistence rps(rc.table);

    rps.insert(rc.id,RUtil::UUID());
    rps.insert(rc.account,accountId);
    rps.insert(rc.operateId,operateId);
    rps.insert(rc.type,type);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rps.sql()))
    {
        return ADD_FRIEND_SENDED;
    }

    return ADD_FRIEND_SENDED_FAILED;
}

/*!
     * @brief 创建用户分组
     * @param[in] db 数据库
     * @param[in] userId User表Id
     * @param[in] groupName 分组名
     * @param[in] isDefault 是否为默认分组，一个用户只有一个默认分组，即在创建时自动创建的分组
     * @return 是否创建成功
     */
bool SQLProcess::createGroup(Database *db, QString userId, QString groupName,QString & groupId, bool isDefault)
{
    groupId = RUtil::UUID();

    DataTable::RGroup rgp;
    RPersistence rps(rgp.table);
    rps.insert(rgp.id,groupId);
    rps.insert(rgp.name,groupName);
    rps.insert(rgp.userId,userId);
    rps.insert(rgp.defaultGroup,(int)isDefault);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rps.sql()))
    {
        return true;
    }

    return false;
}

/*!
     * @brief 更新用户分组，可分别更新联系人分组和群分组信息
     * @param[in] db 数据库
     * @param[in] request 分组操作请求
     * @return 是否更新成功
     */
bool SQLProcess::renameGroup(Database *db, GroupingRequest *request)
{
    QString sql;
    if(request->gtype == GROUPING_FRIEND)
    {
        DataTable::RGroup rgp;
        RUpdate rpd(rgp.table);
        rpd.update(rgp.name,request->groupName);
        rpd.createCriteria().add(Restrictions::eq(rgp.id,request->groupId))
                .add(Restrictions::eq(rgp.userId,request->uuid));
        sql = rpd.sql();
    }
    else if(request->gtype == GROUPING_GROUP)
    {

    }

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        return true;
    }

    return false;
}

/*!
     * @brief 删除用户分组，可分别删除联系人分组和群分组信息
     * @param[in] db 数据库
     * @param[in] request 分组操作请求
     * @return 是否删除成功
     */
bool SQLProcess::deleteGroup(Database *db, GroupingRequest *request)
{
    QString sql;
    if(request->gtype == GROUPING_FRIEND)
    {
        DataTable::RGroup rgp;
        RDelete rde(rgp.table);
        rde.createCriteria().add(Restrictions::eq(rgp.id,request->groupId))
                .add(Restrictions::eq(rgp.userId,request->uuid));
        sql = rde.sql();
    }
    else if(request->gtype == GROUPING_GROUP)
    {

    }

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        return true;
    }

    return false;
}

/*!
     * @brief 建立两个用户之间的关系
     * @param[in] db 数据库
     * @param[in] request 操作请求
     * @return 是否插入成功
     */
bool SQLProcess::establishRelation(Database *db, OperateFriendRequest *request)
{
    db->sqlDatabase().transaction();
    try
    {
        QSqlQuery query(db->sqlDatabase());
        QString idA,nickNameA,idB,nickNameB;

        DataTable::RUser user;
        RSelect rsA(user.table);
        rsA.select(user.id);
        rsA.select(user.nickName);
        rsA.createCriteria().add(Restrictions::eq(user.account,request->accountId));

        if(!query.exec(rsA.sql()))
        {
            throw __LINE__;
        }

        if(query.next())
        {
            idA = query.value(user.id).toString();
            nickNameA = query.value(user.nickName).toString();
        }

        RSelect rsB(user.table);
        rsB.select(user.id);
        rsB.select(user.nickName);
        rsB.createCriteria().add(Restrictions::eq(user.account,request->operateId));

        if(!query.exec(rsB.sql()))
        {
            throw __LINE__;
        }

        if(query.next())
        {
            idB = query.value(user.id).toString();
            nickNameB = query.value(user.nickName).toString();
        }

        QString defaultGroupA = getDefaultGroupByUser(db,idA);
        QString defaultGroupB = getDefaultGroupByUser(db,idB);

        if(defaultGroupA.size() <= 0 ||defaultGroupB.size() <= 0)
        {
            throw __LINE__;
        }

        DataTable::RGroup_User rgu;
        RPersistence rpsA(rgu.table);
        rpsA.insert(rgu.id,RUtil::UUID());
        rpsA.insert(rgu.groupId,defaultGroupA);
        rpsA.insert(rgu.userId,idB);
        rpsA.insert(rgu.remarks,nickNameB);

        if(!query.exec(rpsA.sql()))
        {
            throw __LINE__;
        }

        RPersistence rpsB(rgu.table);
        rpsB.insert(rgu.id,RUtil::UUID());
        rpsB.insert(rgu.groupId,defaultGroupB);
        rpsB.insert(rgu.userId,idA);
        rpsB.insert(rgu.remarks,nickNameA);

        if(!query.exec(rpsB.sql()))
        {
            throw __LINE__;
        }

        db->sqlDatabase().commit();

        return true;
    }
    catch(int value)
    {
        RLOG_ERROR("Execute sql error line:%1",value);
        db->sqlDatabase().rollback();
    }

    return false;
}

//获取好友列表
bool SQLProcess::getFriendList(Database *db, QString accountId, FriendListResponse *response)
{
    DataTable::RUser user;
    DataTable::RGroup group;
    DataTable::RGroup_User groupUser;
    RSelect rst(user.table);
    rst.select(user.id);
    rst.createCriteria().add(Restrictions::eq(user.account,accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()) && query.next())
    {
        QString uid = query.value(user.id).toString();

        RSelect selectGroup(group.table);
        selectGroup.createCriteria().add(Restrictions::eq(group.userId,uid));

        if(query.exec(selectGroup.sql()))
        {
            while(query.next())
            {
                RGroupData * groupData = new RGroupData;
                groupData->groupId = query.value(group.id).toString();
                groupData->groupName = query.value(group.name).toString();
                groupData->isDefault = query.value(group.defaultGroup).toBool();

                QString sql = QString("select ru.account,ru.nickname,ru.signname,ru.face,ru.faceid,rr.remarks from %1 ru left join"
                                      " %2 rr on ru.id = rr.uid where rr.gid = '%3' ").arg(user.table).arg(groupUser.table).arg(groupData->groupId);

                QSqlQuery userQuery(db->sqlDatabase());
                if(userQuery.exec(sql))
                {
                    while(userQuery.next())
                    {
                        SimpleUserInfo simpleUserInfo;

                        simpleUserInfo.accountId = userQuery.value(user.account).toString();
                        simpleUserInfo.nickName = userQuery.value(user.nickName).toString();
                        simpleUserInfo.signName = userQuery.value(user.signName).toString();
                        simpleUserInfo.face = userQuery.value(user.face).toUInt();
                        simpleUserInfo.customImgId = userQuery.value(user.faceId).toString();
                        simpleUserInfo.remarks = userQuery.value(groupUser.remarks).toString();

                        groupData->users.append(simpleUserInfo);
                    }
                }
                response->groups.append(groupData);
            }
            return true;
        }
    }

    return false;
}

void SQLProcess::getUserInfo(Database *db,const QString accountId, UserBaseInfo & userInfo)
{
    DataTable::RUser user;

    RSelect rst(user.table);
    rst.createCriteria().add(Restrictions::eq(user.account,accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        if(query.next())
        {
            userInfo.accountId = accountId;
            userInfo.uuid = query.value(user.id).toString();
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

//根据User表ID查找用户默认分组
QString SQLProcess::getDefaultGroupByUser(Database *db, const QString id)
{
    DataTable::RGroup group;

    RSelect rs(group.table);
    rs.select(group.id);
    rs.createCriteria().add(Restrictions::eq(group.userId,id)).add(Restrictions::eq(group.defaultGroup,1));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rs.sql()))
    {
        if(query.next())
        {
            return query.value(group.id).toString();
        }
    }
    return QString();
}

