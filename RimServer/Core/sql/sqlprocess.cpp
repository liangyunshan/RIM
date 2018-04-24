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
        rst.select(config.table,{config.value});
        rst.createCriteria().add(Restrictions::eq(config.table,config.name,config.accuoutId));

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
    rpc.insert({{user.id,uuid},
                {user.account,QString::number(G_BaseAccountId)},
                {user.password,request->password},
                {user.nickName,request->nickName}});

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpc.sql()))
    {
        id  = QString::number(G_BaseAccountId);

        RUpdate rpd(config.table);
        rpd.update(config.table,config.value,G_BaseAccountId).
                createCriteria().
                add(Restrictions::eq(config.table,config.name,config.accuoutId));

        if(query.exec(rpd.sql())){

        }

        return REGISTER_SUCCESS;
    }

    return REGISTER_FAILED;
}

ResponseLogin SQLProcess::processUserLogin(Database * db,const LoginRequest *request)
{
    DataTable::RUser user;

    RPersistence rpc(user.table);
    QString uuid = RUtil::UUID();
    rpc.insert({{user.id,uuid},
                {user.account,QString::number(G_BaseAccountId)},
                {user.password,request->password},
                {user.nickName,request->accountId}});

    RSelect rst(user.table);
    rst.select(user.table,{user.password}).
            createCriteria().
            add(Restrictions::eq(user.table,user.account,request->accountId));

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
    rpd.update(user.table,{{user.nickName,request->baseInfo.nickName},
                           {user.signName,request->baseInfo.signName},
                           {user.gender,(int)request->baseInfo.sexual},
                           {user.birthDay,QDate::fromString(request->baseInfo.birthday,Constant::Date_Simple)},
                           {user.phone,request->baseInfo.phoneNumber},
                           {user.address,request->baseInfo.address},
                           {user.email,request->baseInfo.email},
                           {user.remark,request->baseInfo.remark},
                           {user.systemIon,request->baseInfo.isSystemIcon},
                           {user.iconId,request->baseInfo.iconId}}).
            createCriteria().
            add(Restrictions::eq(user.table,user.account,request->baseInfo.accountId));

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
    if(request->stype == OperatePerson)
    {
        RSelect rst({user.table});

        rst.select(user.table,{user.account,user.nickName,user.signName,user.systemIon,user.iconId});
        rst.createCriteria().
                add(Restrictions::eq(user.table,user.account,request->accountOrNickName)).
                orr(Restrictions::like(user.table,user.nickName,"%"+request->accountOrNickName+"%"));

        sql = rst.sql();
    }
    else
    {
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

    rps.insert({{rc.id,RUtil::UUID()},
               {rc.account,accountId},
               {rc.operateId,operateId},
               {rc.type,type}});

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rps.sql()))
    {
        return ADD_FRIEND_SENDED;
    }

    return ADD_FRIEND_SENDED_FAILED;
}

/*!
 * @brief 创建联系人分组
 * @param[in] db 数据库
 * @param[in] userId User表Id
 * @param[in] groupName 分组名
 * @param[in] groupId isDefault为true时，由服务器端随机产生id；isDefault为false时，由客户都按用户指定id
 * @param[in] isDefault 是否为默认分组，一个用户只有一个默认分组，即在创建时自动创建的分组
 * @return 是否创建成功
 */
bool SQLProcess::createGroup(Database *db, QString userId, QString groupName, QString groupId, bool isDefault)
{
    DataTable::RGroup rgp;
    RPersistence rps(rgp.table);
    rps.insert({{rgp.id,groupId},
                {rgp.name,groupName},
                {rgp.userId,userId},
                {rgp.defaultGroup,(int)isDefault}});

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rps.sql()))
    {
        return true;
    }

    return false;
}

/*!
 * @brief 创建分组(联系人/群分组)描述信息表
 * @param[in] uuid 用户user表ID
 * @param[in] accountId 用户user表accountid
 * @param[in] groupId 分组ID
 * @return 是否插入成功
 */
bool SQLProcess::createGroupDesc(Database *db, OperateType type, QString uuid, QString accountId, QString groupId)
{
    if(type == OperatePerson){
        DataTable::RGroupDesc rgd;
        RPersistence rps(rgd.table);
        rps.insert({{rgd.id,uuid},
                    {rgd.account,accountId},
                    {rgd.groupids,groupId},
                    {rgd.groupsize,1}});

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rps.sql()))
            return true;
    }else if(type == OperateGroup){
        DataTable::RChatGroupDesc rgd;
        RPersistence rps(rgd.table);
        rps.insert({{rgd.id,uuid},
                    {rgd.account,accountId},
                    {rgd.chatgroupids,groupId},
                    {rgd.chatgroupsize,1}});

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rps.sql()))
            return true;
    }

    return false;
}

/*!
 * @brief 将新创建的分组ID添加至分组信息描述表
 * @param[in] uuid 用户user表ID
 * @param[in] groupId 新创建group id
 * @return 是否插入成功
 */
bool SQLProcess::addGroupToGroupDesc(Database *db, GroupingRequest *request, QString groupId)
{
    if(request->gtype == OperatePerson){
        DataTable::RGroupDesc rgd;
        RSelect rst(rgd.table);
        rst.select(rgd.table,{rgd.groupids,rgd.groupsize})
                .createCriteria()
                .add(Restrictions::eq(rgd.table,rgd.id,request->uuid));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()) && query.next()){
            QString groupIds = query.value(rgd.groupids).toString();
            int groupSize = query.value(rgd.groupsize).toInt();

            groupIds += ",";
            groupIds += groupId;

            RUpdate rud(rgd.table);
            rud.update(rgd.table,{{rgd.groupids,groupIds},{rgd.groupsize,groupSize + 1}})
                    .createCriteria()
                    .add(Restrictions::eq(rgd.table,rgd.id,request->uuid));
            if(query.exec(rud.sql())){
                    return true;
             }
        }
    }else if(request->gtype == OperateGroup){
        DataTable::RChatGroupDesc rcg;
        RSelect rst(rcg.table);
        rst.select(rcg.table,{rcg.chatgroupids,rcg.chatgroupsize})
                .createCriteria()
                .add(Restrictions::eq(rcg.table,rcg.id,request->uuid));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()) && query.next()){
            QString groupIds = query.value(rcg.chatgroupids).toString();
            int groupSize = query.value(rcg.chatgroupsize).toInt();

            groupIds += ",";
            groupIds += groupId;

            RUpdate rud(rcg.table);
            rud.update(rcg.table,{{rcg.chatgroupids,groupIds},{rcg.chatgroupsize,groupSize + 1}})
                    .createCriteria()
                    .add(Restrictions::eq(rcg.table,rcg.id,request->uuid));
            if(query.exec(rud.sql())){
                    return true;
             }
        }
    }
    return false;
}

/*!
 * @brief 从分组信息描述表中删除指定的groupId
 * @attention 默认分组不允许删除，因此默认不处理删除默认分组
 * @param[in] uuid 用户user表ID
 * @param[in] groupId 待删除group id
 * @return 是否删除成功
 */
bool SQLProcess::delGroupInGroupDesc(Database *db, GroupingRequest *request)
{
    if(request->gtype == OperatePerson){
        DataTable::RGroupDesc rgd;
        RSelect rst(rgd.table);
        rst.select(rgd.table,{rgd.groupids,rgd.groupsize})
                .createCriteria()
                .add(Restrictions::eq(rgd.table,rgd.id,request->uuid));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()) && query.next()){
            QString groupIds = query.value(rgd.groupids).toString();
            int groupSize = query.value(rgd.groupsize).toInt();

            QStringList groups = groupIds.split(',');

            if(groups.removeOne(request->groupId)){
                RUpdate rud(rgd.table);
                rud.update(rgd.table,{{rgd.groupids,groups.join(',')},{rgd.groupsize,groupSize - 1}})
                        .createCriteria()
                        .add(Restrictions::eq(rgd.table,rgd.id,request->uuid));
                if(query.exec(rud.sql())){
                        return true;
                 }
            }
        }
    }
    else if(request->gtype == OperateGroup){
        DataTable::RChatGroupDesc rcgd;
        RSelect rst(rcgd.table);
        rst.select(rcgd.table,{rcgd.chatgroupids,rcgd.chatgroupsize})
                .createCriteria()
                .add(Restrictions::eq(rcgd.table,rcgd.id,request->uuid));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()) && query.next()){
            QString groupIds = query.value(rcgd.chatgroupids).toString();
            int groupSize = query.value(rcgd.chatgroupsize).toInt();

            QStringList groups = groupIds.split(',');

            if(groups.removeOne(request->groupId)){
                RUpdate rud(rcgd.table);
                rud.update(rcgd.table,{{rcgd.chatgroupids,groups.join(',')},{rcgd.chatgroupsize,groupSize - 1}})
                        .createCriteria()
                        .add(Restrictions::eq(rcgd.table,rcgd.id,request->uuid));
                if(query.exec(rud.sql())){
                        return true;
                 }
            }
        }
    }
    return true;
}

/*!
 * @brief 分组顺序调整
 * @param[in] uuid 用户user表ID
 * @param[in] groupId 执行排序的group id
 * @param[in] pageIndex 对应group id的新序列
 * @return 是否执行成功
 */
bool SQLProcess::sortGroupInGroupDesc(Database *db, GroupingRequest *request)
{
    if(request->gtype == OperatePerson){
        DataTable::RGroupDesc rgd;
        RSelect rst(rgd.table);
        rst.select(rgd.table,{rgd.groupids,rgd.groupsize})
                .createCriteria()
                .add(Restrictions::eq(rgd.table,rgd.id,request->uuid));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()) && query.next()){
            QString groupIds = query.value(rgd.groupids).toString();
            QStringList groups = groupIds.split(',');

            int oldIndex = groups.indexOf(request->groupId);

            if(oldIndex >=0 && request->groupIndex >= 0 && request->groupIndex < groups.size()){
                groups.removeAt(oldIndex);
                groups.insert(request->groupIndex,request->groupId);

                RUpdate rud(rgd.table);
                rud.update(rgd.table,{{rgd.groupids,groups.join(',')}})
                        .createCriteria()
                        .add(Restrictions::eq(rgd.table,rgd.id,request->uuid));
                if(query.exec(rud.sql())){
                        return true;
                 }
            }
        }
    }else if(request->gtype == OperateGroup){
        DataTable::RChatGroupDesc rcgd;
        RSelect rst(rcgd.table);
        rst.select(rcgd.table,{rcgd.chatgroupids})
                .createCriteria()
                .add(Restrictions::eq(rcgd.table,rcgd.id,request->uuid));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rst.sql()) && query.next()){
            QString groupIds = query.value(rcgd.chatgroupids).toString();
            QStringList groups = groupIds.split(',');

            int oldIndex = groups.indexOf(request->groupId);

            if(oldIndex >=0 && request->groupIndex >= 0 && request->groupIndex < groups.size()){
                groups.removeAt(oldIndex);
                groups.insert(request->groupIndex,request->groupId);

                RUpdate rud(rcgd.table);
                rud.update(rcgd.table,{{rcgd.chatgroupids,groups.join(',')}})
                        .createCriteria()
                        .add(Restrictions::eq(rcgd.table,rcgd.id,request->uuid));
                if(query.exec(rud.sql())){
                        return true;
                 }
            }
        }
    }

    return false;
}

/*!
 * @brief 创建群分组
 * @param[in] db 数据库
 * @param[in] userId User表Id
 * @param[in] groupName 分组名
 * @param[in] groupId isDefault为true时，由服务器端随机产生id；isDefault为false时，由客户都按用户指定id
 * @param[in] isDefault 是否为默认分组，一个用户只有一个默认分组，即在创建时自动创建的分组
 * @return 是否创建成功
 */
bool SQLProcess::createChatGroup(Database *db, QString userId, QString groupName, QString groupId, bool isDefault)
{
    DataTable::RChatGroup rcg;
    RPersistence rps(rcg.table);
    rps.insert({{rcg.id,groupId},
                {rcg.name,groupName},
                {rcg.groupCount,0},
                {rcg.userId,userId},
                {rcg.defaultGroup,(int)isDefault}});

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rps.sql())){
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
    if(request->gtype == OperatePerson)
    {
        DataTable::RGroup rgp;
        RUpdate rpd(rgp.table);
        rpd.update(rgp.table,rgp.name,request->groupName).
                createCriteria().
                add(Restrictions::eq(rgp.table,rgp.id,request->groupId)).
                add(Restrictions::eq(rgp.table,rgp.userId,request->uuid));
        sql = rpd.sql();
    }
    else if(request->gtype == OperateGroup)
    {
        DataTable::RChatGroup rcg;
        RUpdate rpd(rcg.table);
        rpd.update(rcg.table,rcg.name,request->groupName)
                .createCriteria()
                .add(Restrictions::eq(rcg.table,rcg.id,request->groupId))
                .add(Restrictions::eq(rcg.table,rcg.userId,request->uuid));
        sql = rpd.sql();
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
 * @details 若删除的分组中包含联系人，则将联系人移动至默认的分组；再执行删除操作；
 * @param[in] db 数据库
 * @param[in] request 分组操作请求
 * @return 是否删除成功
 */
bool SQLProcess::deleteGroup(Database *db, GroupingRequest *request)
{
    if(request->gtype == OperatePerson)
    {
        QSqlQuery query(db->sqlDatabase());

        //[1]查询分组下联系人数量
        DataTable::RGroup_User rgu;
        RSelect rst(rgu.table);
        rst.select(rgu.table,{rgu.id}).
                createCriteria().
                add(Restrictions::eq(rgu.table,rgu.groupId,request->groupId));

        if(query.exec(rst.sql())){
            //[2]若存在联系人则将联系人移动至默认分组
#if defined(ENABLE_SQL_TRANSACTION)
            if(db->sqlDatabase().transaction()){
#endif
                if(query.next()){
                    QString userDefaultGroupId = getDefaultGroupByUserId(db,OperatePerson,request->uuid);
                    if(userDefaultGroupId.size() > 0){
                        RUpdate rpd(rgu.table);
                        rpd.update(rgu.table,rgu.groupId,userDefaultGroupId).
                                createCriteria().
                                add(Restrictions::eq(rgu.table,rgu.groupId,request->groupId));
                        if(!query.exec(rpd.sql())){
                            return false;
                        }
                    }
                 }

                //[3]删除分组
                DataTable::RGroup rgp;
                RDelete rde(rgp.table);
                rde.createCriteria().
                        add(Restrictions::eq(rgp.table,rgp.id,request->groupId)).
                        add(Restrictions::eq(rgp.table,rgp.userId,request->uuid));

                if(query.exec(rde.sql())){
#if defined(ENABLE_SQL_TRANSACTION)
                       db->sqlDatabase().commit();
#endif
                       return true;
                 }else{
#if defined(ENABLE_SQL_TRANSACTION)
                        db->sqlDatabase().rollback();
#endif
                }
            }else{
                //TODO 20180418
            }
    }else if(request->gtype == OperateGroup){
        QSqlQuery query(db->sqlDatabase());

        //[1]查询分组下联系群数量
        DataTable::RChatGroupRoom rcgr;
        RSelect rst(rcgr.table);
        rst.select(rcgr.table,{rcgr.id}).
                createCriteria().
                add(Restrictions::eq(rcgr.table,rcgr.chatgroupId,request->groupId));

        if(query.exec(rst.sql())){
            //[2]若存在联系人则将联系人移动至默认分组
#if defined(ENABLE_SQL_TRANSACTION)
            if(db->sqlDatabase().transaction()){
#endif
                if(query.next()){
                    QString userDefaultChatGroupId = getDefaultGroupByUserId(db,OperateGroup,request->uuid);
                    if(userDefaultChatGroupId.size() > 0){
                        RUpdate rpd(rcgr.table);
                        rpd.update(rcgr.table,rcgr.chatgroupId,userDefaultChatGroupId)
                                .createCriteria()
                                .add(Restrictions::eq(rcgr.table,rcgr.chatgroupId,request->groupId));
                        if(!query.exec(rpd.sql())){
                            return false;
                        }
                    }
                 }

                //[3]删除分组
                DataTable::RChatGroup rcg;
                RDelete rde(rcg.table);
                rde.createCriteria().
                        add(Restrictions::eq(rcg.table,rcg.id,request->groupId)).
                        add(Restrictions::eq(rcg.table,rcg.userId,request->uuid));

                if(query.exec(rde.sql())){
#if defined(ENABLE_SQL_TRANSACTION)
                       db->sqlDatabase().commit();
#endif
                       return true;
                 }else{
#if defined(ENABLE_SQL_TRANSACTION)
                        db->sqlDatabase().rollback();
#endif
                }
            }else{
                //TODO 20180424
            }
    }
    return false;
}


/*!
 * @brief 测试两个联系人是否已经建立关联关系
 * @details 交替查询A、B双反中是否包含了对方的账号信息;
 * @param[in] db 数据库
 * @param[in] request 分组操作请求
 * @return 连接标识
 */
bool SQLProcess::testTstablishRelation(Database *db, OperateFriendRequest *request)
{
    QStringList alist = getGroupListByUserAccountId(db,request->accountId);
    QStringList blist = getGroupListByUserAccountId(db,request->operateId);

    UserBaseInfo ainfo ,binfo;
    if(getUserInfo(db,request->accountId,ainfo) && getUserInfo(db,request->operateId,binfo)){
        QSqlQuery query(db->sqlDatabase());
        DataTable::RGroup_User rgu;
        foreach(QString s,alist){
            RSelect rst(rgu.table);
            rst.select(rgu.table,{rgu.id})
                    .createCriteria()
                    .add(Restrictions::eq(rgu.table,rgu.groupId,s))
                    .add(Restrictions::eq(rgu.table,rgu.userId,binfo.uuid));
            if(query.exec(rst.sql()) && query.next()){
                return true;
            }
        }

        foreach(QString s,blist){
            RSelect rst(rgu.table);
            rst.select(rgu.table,{rgu.id})
                    .createCriteria()
                    .add(Restrictions::eq(rgu.table,rgu.groupId,s))
                    .add(Restrictions::eq(rgu.table,rgu.userId,ainfo.uuid));
            if(query.exec(rst.sql()) && query.next()){
                return true;
            }
        }
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
        rsA.select(user.table,{user.id,user.nickName}).
                createCriteria().
                add(Restrictions::eq(user.table,user.account,request->accountId));

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
        rsB.select(user.table,{user.id,user.nickName}).
                createCriteria().
                add(Restrictions::eq(user.table,user.account,request->operateId));

        if(!query.exec(rsB.sql()))
        {
            throw __LINE__;
        }

        if(query.next())
        {
            idB = query.value(user.id).toString();
            nickNameB = query.value(user.nickName).toString();
        }

        QString defaultGroupA = getDefaultGroupByUserId(db,OperatePerson,idA);
        QString defaultGroupB = getDefaultGroupByUserId(db,OperatePerson,idB);

        if(defaultGroupA.size() <= 0 ||defaultGroupB.size() <= 0)
        {
            throw __LINE__;
        }

        DataTable::RGroup_User rgu;
        RPersistence rpsA(rgu.table);
        rpsA.insert({{rgu.id,RUtil::UUID()},
                     {rgu.groupId,defaultGroupA},
                     {rgu.userId,idB},
                     {rgu.remarks,nickNameB}});

        if(!query.exec(rpsA.sql()))
        {
            throw __LINE__;
        }

        RPersistence rpsB(rgu.table);
        rpsB.insert({{rgu.id,RUtil::UUID()},
                    {rgu.groupId,defaultGroupB},
                    {rgu.userId,idA},
                    {rgu.remarks,nickNameA}});

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

/*!
 * @brief 获取登陆好友列表
 * @param[in] db 数据库
 * @param[in] accountId 用户账户ID
 * @param[in] response 用户账户ID
 * @return 是否获取成功
 */
bool SQLProcess::getFriendList(Database *db, QString accountId, FriendListResponse *response)
{
    DataTable::RUser user;
    DataTable::RGroup group;
    DataTable::RGroupDesc rgd;
    DataTable::RGroup_User groupUser;
    RSelect rst(user.table);
    rst.select(user.table,{user.id}).
            createCriteria().
            add(Restrictions::eq(user.table,user.account,accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()) && query.next())
    {
        QString uid = query.value(user.id).toString();

        RSelect selectGroupDesc(rgd.table);
        selectGroupDesc.select(rgd.table,{rgd.groupids})
                .createCriteria()
                .add(Restrictions::eq(rgd.table,rgd.id,uid));

        if(query.exec(selectGroupDesc.sql()) && query.next()){
            QStringList groups = query.value(rgd.groupids).toString().split(",");

            foreach(QString groupId,groups){
                RSelect selectGroup(group.table);
                selectGroup.createCriteria()
                           .add(Restrictions::eq(group.table,group.id,groupId));

                if(query.exec(selectGroup.sql())){
                    while(query.next()){
                        RGroupData * groupData = new RGroupData;
                        groupData->groupId = query.value(group.id).toString();
                        groupData->groupName = query.value(group.name).toString();
                        groupData->isDefault = query.value(group.defaultGroup).toBool();

                        RSelect select({user.table,groupUser.table});
                        select.select(user.table,{user.account,user.nickName,user.signName,user.systemIon,user.iconId}).
                                select(groupUser.table,{groupUser.remarks}).
                                on(user.table,user.id,groupUser.table,groupUser.userId).
                                createCriteria().
                                add(Restrictions::eq(groupUser.table,groupUser.groupId,groupData->groupId));

                        QSqlQuery userQuery(db->sqlDatabase());
                        if(userQuery.exec(select.sql())){
                            while(userQuery.next()){
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
                }
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
    rst.createCriteria().
            add(Restrictions::eq(user.table,user.account,accountId));

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
    rst.select(user.table,{user.id}).
            createCriteria().
            add(Restrictions::eq(user.table,user.account,accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()) && query.next())
    {
        QString uid = query.value(user.id).toString();

        RSelect selectGroup(group.table);
        selectGroup.createCriteria().
                add(Restrictions::eq(group.table,group.userId,uid));

        if(query.exec(selectGroup.sql()))
        {
            while(query.next())
            {
                RGroupData * groupData = new RGroupData;
                groupData->groupId = query.value(group.id).toString();
                groupData->groupName = query.value(group.name).toString();
                groupData->isDefault = query.value(group.defaultGroup).toBool();

                RSelect select({user.table,groupUser.table});
                select.select(user.table,{user.account});
                select.on(user.table,user.id,groupUser.table,groupUser.userId);
                select.createCriteria().add(Restrictions::eq(groupUser.table,groupUser.groupId,groupData->groupId));

                QSqlQuery userQuery(db->sqlDatabase());
                if(userQuery.exec(select.sql()))
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

    RUpdate rpd({rgu.table,ru.table});
    rpd.update(rgu.table,rgu.remarks,request->user.remarks).
            on(rgu.table,rgu.userId,ru.table,ru.id).createCriteria().
            add(Restrictions::eq(ru.table,ru.account,request->user.accountId)).
            add(Restrictions::eq(rgu.table,rgu.groupId,request->groupId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpd.sql()))
    {
        return true;
    }
    return false;
}

bool SQLProcess::updateMoveGroupFriend(Database *db, GroupingFriendRequest *request)
{
    DataTable::RGroup_User rgu;
    DataTable::RUser ru;

    RUpdate rpd({rgu.table,ru.table});
    rpd.update(rgu.table,rgu.groupId,request->groupId).
            on(rgu.table,rgu.userId,ru.table,ru.id).
            createCriteria().
            add(Restrictions::eq(rgu.table,rgu.groupId,request->oldGroupId)).
            add(Restrictions::eq(ru.table,ru.account,request->user.accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rpd.sql()))
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
            rde.createCriteria().add(Restrictions::eq(rgu.table,rgu.groupId,request->groupId))
                    .add(Restrictions::eq(rgu.table,rgu.userId,otherSideUserInfo.uuid));

            if(!query.exec(rde.sql()))
            {
                throw "delete other friend from my group error!";
            }

            //2.获取自己的ID信息
            DataTable::RGroup rgp;
            DataTable::RUser rus;

            RSelect select({rus.table,rgp.table});
            select.select(rus.table,{rus.id,rus.account}).
                    on(rus.table,rus.id,rgp.table,rgp.userId).
                    createCriteria().
                    add(Restrictions::eq(rgp.table,rgp.id,request->groupId));

            QString userId;
            if(query.exec(select.sql()) && query.next())
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
                   rsg.select(rgu.table,{rgu.id});
                   rsg.createCriteria().
                           add(Restrictions::eq(rgu.table,rgu.groupId,*iter)).
                           add(Restrictions::eq(rgu.table,rgu.userId,userId));

                   if(query.exec(rsg.sql()) && query.next())
                   {
                       query.clear();
                       rde.clearRestrictions();
                       rde.createCriteria().
                               add(Restrictions::eq(rgu.table,rgu.groupId,*iter)).
                               add(Restrictions::eq(rgu.table,rgu.userId,userId));

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
 * @brief 获取指定用户的群分组列表
 * @param[in] db 数据库
 * @param[in] userId 缓存请求容器
 * @param[in] response 请求的结果列表
 */
bool SQLProcess::getGroupList(Database *db, const QString & userId, ChatGroupListResponse *response)
{
    DataTable::RChatGroupDesc rcgd;
    RSelect rst(rcgd.table);
    rst.select(rcgd.table,{rcgd.chatgroupids})
            .createCriteria()
            .add(Restrictions::eq(rcgd.table,rcgd.id,userId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()) && query.next()){
        QStringList groupIds = query.value(rcgd.chatgroupids).toString().split(",");

        foreach(QString gid,groupIds){

            DataTable::RChatGroup rcg;
            RSelect selectChatGroup(rcg.table);
            selectChatGroup.createCriteria()
                    .add(Restrictions::eq(rcg.table,rcg.id,gid));

            if(query.exec(selectChatGroup.sql()) && query.next()){
                RChatGroupData * groupData = new RChatGroupData;
                groupData->groupId = query.value(rcg.id).toString();
                groupData->groupName = query.value(rcg.name).toString();
                groupData->isDefault = query.value(rcg.defaultGroup).toBool();

                DataTable::RChatGroupRoom rcgr;
                DataTable::RChatRoom rcr;
                RSelect selectChatGroupRoom ({rcgr.table,rcr.table});
                selectChatGroupRoom.select(rcgr.table,{rcgr.chatroomId,rcgr.remarks,rcgr.messNotifyLevel})
                        .select(rcr.table,{rcr.chatId,rcr.systemIon,rcr.iconId})
                        .on(rcgr.table,rcgr.chatroomId,rcr.table,rcr.id)
                        .createCriteria()
                        .add(Restrictions::eq(rcgr.table,rcgr.chatgroupId,gid));

                if(query.exec(selectChatGroupRoom.sql())){
                    while(query.next()){
                        SimpleChatInfo * chatInfo = new SimpleChatInfo;
                        chatInfo->chatRoomId = query.value(rcgr.chatroomId).toString();
                        chatInfo->chatId = query.value(rcr.chatId).toString();
                        chatInfo->remarks = query.value(rcgr.remarks).toString();
                        chatInfo->messNotifyLevel = (ChatMessNotifyLevel)query.value(rcgr.messNotifyLevel).toInt();
                        chatInfo->isSystemIcon = query.value(rcr.systemIon).toBool();
                        chatInfo->iconId = query.value(rcr.iconId).toString();

                        groupData->chatGroups.push_back(chatInfo);
                    }
                }

                response->groups.push_back(groupData);
            }
        }
        return true;
    }

    return false;
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
    rst.select(requestCache.table,{requestCache.account,requestCache.operateId,requestCache.type}).
            createCriteria().
            add(Restrictions::eq(requestCache.table,requestCache.operateId,accountId));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            AddFriendRequest cacheRequest;
            cacheRequest.accountId = query.value(requestCache.account).toString();
            cacheRequest.operateId = query.value(requestCache.operateId).toString();
            cacheRequest.stype = (OperateType)query.value(requestCache.type).toInt();
            requests.append(cacheRequest);
        }

        if(requests.size() > 0)
        {
            RDelete rde(requestCache.table);
            rde.createCriteria().add(Restrictions::eq(requestCache.table,requestCache.operateId,accountId));
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
    rst.select(userchatcache.table,{userchatcache.account,userchatcache.otherSideId,userchatcache.data,userchatcache.time,userchatcache.msgType
               ,userchatcache.textId,userchatcache.textType,userchatcache.encryption,userchatcache.compress}).
            createCriteria().
            add(Restrictions::eq(userchatcache.table,userchatcache.otherSideId,accountId));

    QSqlQuery query(db->sqlDatabase());
    qDebug()<<rst.sql();
    if(query.exec(rst.sql()))
    {
        while(query.next())
        {
            TextRequest textCache;
            textCache.accountId = query.value(userchatcache.account).toString();
            textCache.type = OperatePerson;
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
            rde.createCriteria().add(Restrictions::eq(userchatcache.table,userchatcache.otherSideId,accountId));
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
    rps.insert({{userchatcache.account,request->accountId},
               {userchatcache.otherSideId,request->otherSideId},
               {userchatcache.data,request->sendData},
               {userchatcache.time,request->timeStamp},
               {userchatcache.msgType,request->msgCommand},
               {userchatcache.textId,request->textId},
               {userchatcache.textType,request->textType},
               {userchatcache.encryption,request->isEncryption},
               {userchatcache.compress,request->isCompress}});

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
    rst.select(rfile.table,{rfile.id});
    rst.createCriteria().
            add(Restrictions::eq(rfile.table,rfile.src,request->accountId)).
            add(Restrictions::eq(rfile.table,rfile.dst,request->otherId)).
            add(Restrictions::eq(rfile.table,rfile.fileName,request->fileName));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql()) && query.next())
    {
        return true;
    }

    RSelect rst1(rfile.table);
    rst1.select(rfile.table,{rfile.id,rfile.quoteNum});
    rst1.createCriteria().add(Restrictions::eq(rfile.table,rfile.md5,request->md5));
    if(query.exec(rst1.sql())&& query.next())
    {
        QString originId = query.value(rfile.id).toString();
        int quoteNum = query.value(rfile.quoteNum).toInt();

        if(originId.size() > 0)
        {
            RPersistence rps(rfile.table);
            rps.insert({{rfile.id,request->fileId},
                       {rfile.quoteId,originId},
                       {rfile.quoteNum,0},
                       {rfile.fileName,request->fileName},
                       {rfile.src,request->accountId},
                       {rfile.dst,request->otherId},
                       {rfile.dtime,QDateTime::currentDateTime()},
                       {rfile.fileSize,request->size}});

            if(query.exec(rps.sql()))
            {
                RUpdate rud(rfile.table);
                rud.update(rfile.table,rfile.quoteNum,quoteNum+1).
                        createCriteria().
                        add(Restrictions::eq(rfile.table,rfile.id,originId));
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

    rst.createCriteria().
            add(Restrictions::eq(rfile.table,rfile.md5,fileMd5));

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
    rs.insert({{rfile.id,desc->fileId},
              {rfile.md5,desc->md5},
              {rfile.quoteNum,0},
              {rfile.fileName,desc->fileName},
              {rfile.src,desc->accountId},
              {rfile.dst,desc->otherId},
              {rfile.dtime,QDateTime::currentDateTime()},
              {rfile.fileSize,desc->size},
              {rfile.filePath,RGlobal::G_FILE_UPLOAD_PATH}});

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

    rst.createCriteria().
            add(Restrictions::eq(rfile.table,rfile.id,request->fileId));

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

    rst.createCriteria().
            add(Restrictions::eq(rfile.table,rfile.id,request->fileId));

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
                rst1.select(rfile.table,{rfile.md5,rfile.fileName,rfile.filePath}).
                        createCriteria().
                        add(Restrictions::eq(rfile.table,rfile.id,quoteId));
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


/*!
 * @brief 根据User表ID查找用户默认联系人或群分组
 * @param[in] db:Database * 数据库
 * @param[in] type:SearchType 操作类型，支持对用户分组或联系人分组
 * @param[in] userId:QString 用户user id
 * @return 默认分组ID
 */
QString SQLProcess::getDefaultGroupByUserId(Database *db, OperateType type, const QString userId)
{
    if(type == OperatePerson){
        DataTable::RGroup group;

        RSelect rs(group.table);
        rs.select(group.table,{group.id});
        rs.createCriteria().
                add(Restrictions::eq(group.table,group.userId,userId)).
                add(Restrictions::eq(group.table,group.defaultGroup,1));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rs.sql())&&query.next())
        {
            return query.value(group.id).toString();
        }
    }else if(type == OperateGroup){
        DataTable::RChatGroup group;

        RSelect rs(group.table);
        rs.select(group.table,{group.id});
        rs.createCriteria().
                add(Restrictions::eq(group.table,group.userId,userId)).
                add(Restrictions::eq(group.table,group.defaultGroup,1));

        QSqlQuery query(db->sqlDatabase());
        if(query.exec(rs.sql()) && query.next())
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

    RSelect select({group.table,user.table});
    select.select(group.table,{group.id}).
            on(group.table,group.userId,user.table,user.id).
            createCriteria().
            add(Restrictions::eq(user.table,user.account,id)).
            add(Restrictions::eq(group.table,group.defaultGroup,1));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(select.sql()))
    {
        if(query.next())
        {
            return query.value(group.id).toString();
        }
    }
    return QString();
}

/*!
 * @brief 获取指定用户的分组ID
 * @param[in] db 数据库
 * @param[in] id user表id
 * @return 分组列表
 */
QStringList SQLProcess::getGroupListByUserId(Database *db, const QString id)
{
    QStringList result;
    DataTable::RGroup rgroup;
    RSelect rst({rgroup.table});
    rst.select(rgroup.table,{rgroup.id})
            .createCriteria()
            .add(Restrictions::eq(rgroup.table,rgroup.userId,id));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql())){
        while(query.next()){
            result<<query.value(rgroup.id).toString();
        }
    }

    return result;
}

/*!
 * @brief 获取指定用户的分组ID
 * @param[in] db 数据库
 * @param[in] id user表AccountId
 * @return 分组列表
 */
QStringList SQLProcess::getGroupListByUserAccountId(Database *db, const QString id)
{
    QStringList result;

    DataTable::RUser ruser;
    DataTable::RGroup rgroup;
    RSelect rst({rgroup.table,ruser.table});
    rst.select(rgroup.table,{rgroup.id})
            .on(rgroup.table,rgroup.userId,ruser.table,ruser.id)
            .createCriteria()
            .add(Restrictions::eq(ruser.table,ruser.account,id));

    QSqlQuery query(db->sqlDatabase());
    if(query.exec(rst.sql())){
        while(query.next()){
            result<<query.value(rgroup.id).toString();
        }
    }

    return result;
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
    rst.select(group.table,{group.id});
    rst.createCriteria().
            add(Restrictions::eq(group.table,group.userId,id));

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

