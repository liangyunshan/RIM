#include "sqlprocess.h"

#include <QString>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlResult>
#include <QMutex>
#include <QDate>
#include <QDateTime>
#include <QSqlRecord>

#include "datatable.h"
#include "sql/database.h"
#include "Util/rutil.h"
#include "constants.h"
#include "rpersistence.h"
#include "datastruct.h"
#include "Util/rlog.h"
#include "global.h"

QMutex ACCOUNT_LOCK;

int G_BaseAccountId = 0;

#define SQL_ERROR -1

/*!
 *  @brief  开启事务宏
 *  @details 在对某些数据库操作时，可能不支持事务，需要动态设置。 @n
 *           当Qt的版本和Mysql的驱动版本不一致时，存在Qt不支持Mysql事务的情况。 @n
 */
//#define ENABLE_SQL_TRANSACTION

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
    rpd.update(user.systemIon,request->baseInfo.isSystemIcon);
    rpd.update(user.iconId,request->baseInfo.iconId);

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
        rst.select(user.systemIon);
        rst.select(user.iconId);
        rst.createCriteria().add(Restrictions::eq(user.account,request->accountOrNickName))
                .orr(Restrictions::like(user.nickName,"%"+request->accountOrNickName+"%"));

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
                result.isSystemIcon = query.value(user.systemIon).toBool();
                result.iconId = query.value(user.iconId).toString();
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

/*!
 * @brief 存储好友响应结果至数据库
 * @param[in] db 数据库
 * @param[in] accountId 用户ID
 * @param[in] operateId 待操作用户ID
 * @param[in] type 操作类型 @link ResponseFriendApply @endlink
 * @return 是否插入成功
 */
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

        QString defaultGroupA = getDefaultGroupByUserId(db,idA);
        QString defaultGroupB = getDefaultGroupByUserId(db,idB);

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

                QString sql = QString("select ru.account,ru.nickname,ru.signname,ru.SYSTEMICON,ru.ICONID,rr.remarks from %1 ru left join"
                                      " %2 rr on ru.id = rr.uid where rr.gid = '%3' ").arg(user.table).arg(groupUser.table).arg(groupData->groupId);

                QSqlQuery userQuery(db->sqlDatabase());
                if(userQuery.exec(sql))
                {
                    while(userQuery.next())
                    {
                        SimpleUserInfo * simpleUserInfo = new SimpleUserInfo;

                        simpleUserInfo->accountId = userQuery.value(user.account).toString();
                        simpleUserInfo->nickName = userQuery.value(user.nickName).toString();
                        simpleUserInfo->signName = userQuery.value(user.signName).toString();
                        simpleUserInfo->isSystemIcon = userQuery.value(user.systemIon).toBool();
                        simpleUserInfo->iconId = userQuery.value(user.iconId).toString();
                        simpleUserInfo->remarks = userQuery.value(groupUser.remarks).toString();

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

/*!
 * @brief 根据用户账户ID获取用户基本信息
 * @param[in] db 数据库
 * @param[in] accountId 用户账户ID
 * @param[out] userInfo  用户基本信息
 * @return 无
 */
bool SQLProcess::getUserInfo(Database *db,const QString accountId, UserBaseInfo & userInfo)
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
            userInfo.isSystemIcon = query.value(user.systemIon).toBool();
            userInfo.iconId = query.value(user.iconId).toString();

            return true;
        }
    }
    return false;
}

void SQLProcess::getFriendAccountList(Database *db, const QString accountId, QList<QString> &friendList)
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

                QString sql = QString("select ru.account from %1 ru left join"
                                      " %2 rr on ru.id = rr.uid where rr.gid = '%3' ").arg(user.table).arg(groupUser.table).arg(groupData->groupId);

                QSqlQuery userQuery(db->sqlDatabase());
                if(userQuery.exec(sql))
                {
                    while(userQuery.next())
                    {
                        friendList.append(userQuery.value(user.account).toString());
                    }
                }
            }
        }
    }
}

bool SQLProcess::updateGroupFriendInfo(Database *db, GroupingFriendRequest *request)
{
    DataTable::RGroup_User rgu;
    DataTable::RUser ru;

    QString sql = QString("update %1 ru left join %2 rr on ru.uid = rr.id  set ru.remarks = '%3' where ru.gid = '%4' "
                          "and rr.account = '%5'").arg(rgu.table,ru.table,request->user.remarks,request->groupId,request->user.accountId);
    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        return true;
    }
    return false;
}

bool SQLProcess::updateMoveGroupFriend(Database *db, GroupingFriendRequest *request)
{
    DataTable::RGroup_User rgu;
    DataTable::RUser ru;

    QString sql = QString("update %1 ru left join %2 rr on ru.uid = rr.id  set ru.gid = '%3' where ru.gid = '%4' "
                          "and rr.account = '%5'").arg(rgu.table,ru.table,request->groupId,request->oldGroupId,request->user.accountId);
    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        return true;
    }
    return false;
}

/*!
 * @brief 删除用户好友
 * @details 1.先解除双方的好友关系；2.若好友在线，则向好友推送删除的结果信息，确保将好友列表中的自己移除。
 * @param[in] db 数据库
 * @param[in] requests 缓存请求容器
 * @param[out] otherUserGroupId 自己在对方用户分组中的ID
 * @return 是否删除成功
 */
bool SQLProcess::deleteFriend(Database *db, GroupingFriendRequest *request,QString & accountId,QString & otherUserGroupId)
{
    try
    {
        QSqlQuery query(db->sqlDatabase());

        //TODO 将处理放入事务中处理，当前mysql的版本与qt的库存在不一致，暂不支持事务，因此需要重新编译mysql驱动
#if defined(ENABLE_SQL_TRANSACTION)
        if(db->sqlDatabase().transaction())
#endif
        {
            //1.从己方列表中删除对方
            UserBaseInfo otherSideUserInfo;
            if(!getUserInfo(db,request->user.accountId,otherSideUserInfo))
            {
                throw "get other userbaseinfo errror";
            }

            DataTable::RGroup_User rgu;
            RDelete rde(rgu.table);
            rde.createCriteria().add(Restrictions::eq(rgu.groupId,request->groupId))
                    .add(Restrictions::eq(rgu.userId,otherSideUserInfo.uuid));

            if(!query.exec(rde.sql()))
            {
                throw "delete other friend from my group error!";
            }

            //2.获取自己的ID信息
            DataTable::RGroup rgp;
            DataTable::RUser rus;
            QString sql = QString("select u.id,u.account from %1 u left join  %2 g on u.id = g.uid where g.id = '%3'")
                    .arg(rus.table,rgp.table,request->groupId);

            QString userId;
            if(query.exec(sql) && query.next())
            {
               userId = query.value(rus.id).toString();
               accountId = query.value(rus.account).toString();
            }
            else
            {
                throw "get userbaseinfo error!";
            }

            //3.获取对方分组ID
            QStringList groups = getGroupsById(db,otherSideUserInfo.uuid);
            if(groups.size() > 0)
            {
               QStringList::iterator iter =  groups.begin();
               while(iter != groups.end())
               {
                   RSelect rsg(rgu.table);
                   rsg.select(rgu.id);
                   rsg.createCriteria().add(Restrictions::eq(rgu.groupId,*iter))
                           .add(Restrictions::eq(rgu.userId,userId));

                   if(query.exec(rsg.sql()) && query.next())
                   {
                       query.clear();
                       rde.clearRestrictions();
                       rde.createCriteria().add(Restrictions::eq(rgu.groupId,*iter))
                               .add(Restrictions::eq(rgu.userId,userId));

                       otherUserGroupId = *iter;
                       if(!query.exec(rde.sql()))
                       {
                           throw "delete me from other group error!";
                       }

                       break;
                   }
                   iter++;
               }
            }

#if defined(ENABLE_SQL_TRANSACTION)
            db->sqlDatabase().commit();
#endif
        }
#if defined(ENABLE_SQL_TRANSACTION)
        else
        {
            throw "Current database can't support transaction!";
        }
#endif
    }
    catch(const char * p)
    {
#if defined(ENABLE_SQL_TRANSACTION)
        db->sqlDatabase().rollback();
#endif
        RLOG_ERROR(p);
        return false;
    }

    return true;
}

/*!
 * @brief 加载缓存的系统通知消息
 * @param[in] db 数据库
 * @param[in] accountdId 账户ID
 * @param[in] requests 缓存请求容器
 * @return 是否加载成功
 * @attention 离线消息数量大于0时，认为加载成功
 */
bool SQLProcess::loadSystemCache(Database *db, QString accountId, QList<AddFriendRequest> &requests)
{
    DataTable::RequestCache requestCache;
    RSelect rst(requestCache.table);
    rst.select(requestCache.account);
    rst.select(requestCache.operateId);
    rst.select(requestCache.type);
    rst.createCriteria().add(Restrictions::eq(requestCache.operateId,accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            AddFriendRequest cacheRequest;
            cacheRequest.accountId = query.value(requestCache.account).toString();
            cacheRequest.operateId = query.value(requestCache.operateId).toString();
            cacheRequest.stype = (SearchType)query.value(requestCache.type).toInt();
            requests.append(cacheRequest);
        }

        if(requests.size() > 0)
        {
            RDelete rde(requestCache.table);
            rde.createCriteria().add(Restrictions::eq(requestCache.operateId,accountId));
            if(query.exec(rde.sql()))
            {
                return true;
            }
        }
    }

    return false;
}

/*!
 * @brief 加载缓存的联系人消息
 * @param[in] db 数据库
 * @param[in] accountdId 账户ID
 * @param[in] textResponse 离线消息存储
 * @return 是否加载成功
 * @attention [1]离线消息数量大于0时，认为加载成功 @n
 *            [2]加载用户A消息时，需要将A的ID作为目的消息ID查询 @n
 */
bool SQLProcess::loadChatCache(Database *db, QString accountId, QList<TextRequest>& textResponse)
{
    DataTable::RUserChatCache userchatcache;
    RSelect rst(userchatcache.table);
    rst.select(userchatcache.account);
    rst.select(userchatcache.otherSideId);
    rst.select(userchatcache.data);
    rst.select(userchatcache.time);
    rst.select(userchatcache.msgType);
    rst.select(userchatcache.textId);
    rst.select(userchatcache.textType);
    rst.select(userchatcache.encryption);
    rst.select(userchatcache.compress);
    rst.createCriteria().add(Restrictions::eq(userchatcache.otherSideId,accountId));

    QSqlQuery query(db->sqlDatabase());
    qDebug()<<rst.sql();
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            TextRequest textCache;
            textCache.accountId = query.value(userchatcache.account).toString();
            textCache.type = SearchPerson;
            //NOTE 注意
            textCache.otherSideId = query.value(userchatcache.otherSideId).toString();
            textCache.sendData = query.value(userchatcache.data).toString();
            textCache.timeStamp = query.value(userchatcache.time).toLongLong();
            textCache.msgCommand = (MsgCommand)query.value(userchatcache.msgType).toInt();
            textCache.textId = query.value(userchatcache.textId).toString();
            textCache.textType = (TextType)query.value(userchatcache.textType).toInt();
            textCache.isEncryption = query.value(userchatcache.encryption).toBool();
            textCache.isCompress = query.value(userchatcache.compress).toBool();
            textResponse.append(textCache);
        }

        if(textResponse.size() > 0)
        {
            RDelete rde(userchatcache.table);
            rde.createCriteria().add(Restrictions::eq(userchatcache.otherSideId,accountId));
//            if(query.exec(rde.sql()))
            {
                return true;
            }
        }
    }

    return false;
}

/*!
 * @brief 保存用户间对话信息
 * @param[in] db 数据库
 * @param[in] request 文本请求
 * @return 是否保存成功
 */
bool SQLProcess::saveUserChat2Cache(Database *db, TextRequest *request)
{
    DataTable::RUserChatCache userchatcache;
    RPersistence rps(userchatcache.table);
    rps.insert(userchatcache.account,request->accountId);
    rps.insert(userchatcache.otherSideId,request->otherSideId);
    rps.insert(userchatcache.data,request->sendData);
    rps.insert(userchatcache.time,request->timeStamp);
    rps.insert(userchatcache.msgType,request->msgCommand);
    rps.insert(userchatcache.textId,request->textId);
    rps.insert(userchatcache.textType,request->textType);
    rps.insert(userchatcache.encryption,request->isEncryption);
    rps.insert(userchatcache.compress,request->isCompress);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rps.sql()))
    {
        return true;
    }

    return false;
}

/*!
 * @brief 为已经存在的文件增加引用记录
 * @details 1.查询是否存在文件名、发送源、目的地一致的数据记录，若存在则直接返回true，说明文件已经存在； @n
 *          2.若不存在，查询MD5对应的记录的信息，主要为ID信息； @n
 *          3.插入当前数据的记录信息，MD5一栏为空，QUOTEID一栏为上面查询的ID，其它为本次的request信息; @n
 *          4.更新步骤2中ID的QuoteNum，将其+1并保存； @n
 * @param[in] db 数据库连接
 * @param[in] request 文件传输请求
 * @return 是否引用成功
 */
bool SQLProcess::addQuoteFile(Database *db, const FileItemRequest *request)
{
    DataTable::RFile rfile;

    RSelect rst(rfile.table);
    rst.select(rfile.id);
    rst.createCriteria().add(Restrictions::eq(rfile.src,request->accountId))
            .add(Restrictions::eq(rfile.dst,request->otherId)).add(Restrictions::eq(rfile.fileName,request->fileName));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()) && query.next())
    {
        return true;
    }

    RSelect rst1(rfile.table);
    rst1.select(rfile.id);
    rst1.select(rfile.quoteNum);
    rst1.createCriteria().add(Restrictions::eq(rfile.md5,request->md5));
    if(query.exec(rst1.sql())&& query.next())
    {
        QString originId = query.value(rfile.id).toString();
        int quoteNum = query.value(rfile.quoteNum).toInt();

        if(originId.size() > 0)
        {
            RPersistence rps(rfile.table);
            rps.insert(rfile.id,request->fileId);
            rps.insert(rfile.quoteId,originId);
            rps.insert(rfile.quoteNum,0);
            rps.insert(rfile.fileName,request->fileName);
            rps.insert(rfile.src,request->accountId);
            rps.insert(rfile.dst,request->otherId);
            rps.insert(rfile.dtime,QDateTime::currentDateTime());
            rps.insert(rfile.fileSize,request->size);

            if(query.exec(rps.sql()))
            {
                RUpdate rud(rfile.table);
                rud.update(rfile.quoteNum,quoteNum+1);
                rud.createCriteria().add(Restrictions::eq(rfile.id,originId));
                if(query.exec(rud.sql()))
                {
                    return true;
                }
            }
        }
    }


    return false;
}

/*!
 * @brief 查询数据库中是否存在此md5的文件
 * @param[in] db 数据库连接
 * @param[in] fileMd5 文件md5
 * @return 是否存在
 */
bool SQLProcess::queryFile(Database *db, const QString &fileMd5)
{
    DataTable::RFile rfile;
    RSelect rst(rfile.table);

    rst.createCriteria().add(Restrictions::eq(rfile.md5,fileMd5));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        if(query.next())
        {
            return true;
        }
    }

    return false;
}

/*!
 * @brief 向数据库中插入用户发送文件的描述信息
 * @param[in] db 数据库链接
 * @param[in] desc 基本文件描述
 * @param[in/out] fileId 文件在数据库中唯一ID
 * @return 是否插入成功
 */
bool SQLProcess::addFile(Database *db, ServerNetwork::FileRecvDesc *desc)
{
    DataTable::RFile rfile;
    RPersistence rs(rfile.table);
    rs.insert(rfile.id,desc->fileId);
    rs.insert(rfile.md5,desc->md5);
    rs.insert(rfile.quoteNum,0);
    rs.insert(rfile.fileName,desc->fileName);
    rs.insert(rfile.src,desc->accountId);
    rs.insert(rfile.dst,desc->otherId);
    rs.insert(rfile.dtime,QDateTime::currentDateTime());
    rs.insert(rfile.fileSize,desc->size);
    rs.insert(rfile.filePath,RGlobal::G_FILE_UPLOAD_PATH);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rs.sql()))
    {
        return true;
    }

    return false;
}

bool SQLProcess::getFileInfo(Database *db, SimpleFileItemRequest *request, FileItemRequest *response)
{
    DataTable::RFile rfile;
    RSelect rst(rfile.table);

    rst.createCriteria().add(Restrictions::eq(rfile.id,request->fileId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        if(query.next())
        {
            response->fileId = query.value(rfile.id).toString();
            response->fileName = query.value(rfile.fileName).toString();
            response->size = query.value(rfile.fileSize).toUInt();
            response->md5 = query.value(rfile.md5).toString();
            response->accountId = query.value(rfile.src).toString();
            response->otherId = query.value(rfile.dst).toString();
            return true;
        }
    }

    return false;
}

/*!
 * @brief 获取解引用的文件信息
 * @details 下载文件时，需先检测当前ID是否引用了其它ID文件，若引用则返回引用的文件名，作为下载时打开的文件。
 * @return 是否成功获取文件信息
 */
bool SQLProcess::getDereferenceFileInfo(Database *db, SimpleFileItemRequest *request, FileItemInfo *itemInfo)
{
    DataTable::RFile rfile;
    RSelect rst(rfile.table);

    rst.createCriteria().add(Restrictions::eq(rfile.id,request->fileId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        if(query.next())
        {
            itemInfo->md5 = query.value(rfile.md5).toString();
            itemInfo->fileId = query.value(rfile.id).toString();
            itemInfo->fileName = query.value(rfile.fileName).toString();
            itemInfo->size = query.value(rfile.fileSize).toUInt();
            itemInfo->accountId = query.value(rfile.src).toString();
            itemInfo->otherId = query.value(rfile.dst).toString();
            itemInfo->filePath = query.value(rfile.filePath).toString();
            QString quoteId = query.value(rfile.quoteId).toString();

            //存在引用，查找真实的MD5和FileName
            if(itemInfo->md5.size() <= 0 && quoteId.size() > 0)
            {
                RSelect rst1(rfile.table);
                rst1.select(rfile.md5);
                rst1.select(rfile.fileName);
                rst1.select(rfile.filePath);
                rst1.createCriteria().add(Restrictions::eq(rfile.id,quoteId));
                if(query.exec(rst1.sql()) && query.next())
                {
                    itemInfo->md5 = query.value(rfile.md5).toString();
                    itemInfo->fileName = query.value(rfile.fileName).toString();
                    itemInfo->filePath = query.value(rfile.filePath).toString();

                    return true;
                }
            }
            else
            {
                return true;
            }
        }
    }

    return false;
}


//根据User表ID查找用户默认分组
QString SQLProcess::getDefaultGroupByUserId(Database *db, const QString id)
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

/*!
 * @brief 获取用户默认分组ID
 * @param[in] db 数据库
 * @param[in] id 用户账户ID
 * @return 默认分组ID
 */
QString SQLProcess::getDefaultGroupByUserAccountId(Database *db, const QString id)
{
    DataTable::RGroup group;
    DataTable::RUser user;

    QString sql = QString("select g.id from %1 g left join %2 u on g.uid = u.id where %3 = %4 and %5 = 1 ")
            .arg(group.table,user.table,user.account,id,group.defaultGroup);

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(sql))
    {
        if(query.next())
        {
            return query.value(group.id).toString();
        }
    }
    return QString();
}

/*!
 * @brief 根据ID获取用户所有的分组
 * @param[in] db 数据库
 * @param[in] id User表中的ID
 * @return 用户所有分组ID
 * @warning id不是登陆ID，此id是数据库中主键，由随机产生。
 */
QStringList SQLProcess::getGroupsById(Database *db, const QString id)
{
    DataTable::RGroup group;
    RSelect rst(group.table);
    rst.select(group.id);
    rst.createCriteria().add(Restrictions::eq(group.userId,id));

    QStringList list;
    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            list.append(query.value(group.id).toString());
        }
    }
    return list;
}

