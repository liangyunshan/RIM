/*!
 *  @brief     数据库表
 *  @details   分离硬编码，方便数据字段名称的调整。
 *  @author    wey
 *  @version   1.0
 *  @date      2018.04.20
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATATABLE_H
#define DATATABLE_H

#include <QString>

namespace DataTable
{

extern const QString SQLHistoryChat;
extern const QString SQLChatList;
extern const QString SQLChatRecord;

/*!
 *  @brief  历史对话记录表
 */
class RHistoryChat
{
public:
    RHistoryChat();
    const QString table;
    const QString id;                       //id,int
    const QString type;                     //记录类型(群、个人、系统通知)
    const QString accountId;                //聊天账户
    const QString nickName;                 //个人昵称、群名称
    const QString dtime;                    //日期
    const QString lastRecord;               //最后记录
    const QString isTop;                    //是否置顶
    const QString systemIon;                //是否为系统头像，默认为系统头像true
    const QString iconId;                   //图片索引
};

/*!
 *  @brief  聊天清单
 */
class RChatList
{
public:
    RChatList();
    const QString table;
    const QString id;                   /*!< 记录id，int自增 */
    const QString accountId;            /*!< 对方accountId */
    const QString firstChatTime;        /*!< 聊天记录开始时间 */
};

/*!
 *  @brief  聊天记录数据表
 */
class RChatRecord
{
public:
    RChatRecord();
//    const QString table;
    QString table;
    const QString id;                   /*!< 记录id，int自动增长 */
    const QString accountId;            /*!< 对方accountId */
    const QString nickName;             /*!< 对方昵称 */
    const QString time;                 /*!< 时间戳 */
    const QString type;                 /*!< 信息类型:文本信息、语音信息、图片信息等 */
    const QString dateTime;             /*!< 日期时间 */
    const QString serialNo;             /*!< 流水号 */
    const QString data;                 /*!< 对话内容 */
    bool initTable(const QString &name);
};

class RChatSerialNo
{
public:
    RChatSerialNo();
    bool initTable(const QString &name);
    QString table;
    const QString id;                   /*!< 记录id，int自动增长 */
    const QString serialno;            /*!< 最新serialno */
};

}

#endif // TABLE_H
