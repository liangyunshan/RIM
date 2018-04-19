/*!
 *  @brief     数据库表
 *  @details   分离硬编码，方便数据字段名称的调整。
 *  @file      datatable.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATATABLE_H
#define DATATABLE_H

#include <QString>

namespace DataTable
{

//历史对话记录表
class RHistoryChat
{
public:
    RHistoryChat();
    const QString table;
    const QString type;                     //记录类型(群、个人、系统通知)
    const QString accountId;                //聊天账户
    const QString nickName;                 //个人昵称、群名称
    const QString dtime;                    //日期
    const QString lastRecord;               //最后记录
    const QString isTop;                    //是否置顶
    const QString systemIon;                //是否为系统头像，默认为系统头像true
    const QString iconId;                   //图片索引
};


}

/*!

  @code
    SET FOREIGN_KEY_CHECKS=0;

    -- ----------------------------
    -- Table structure for `rchatroom`
    -- ----------------------------
    DROP TABLE IF EXISTS `rchatroom`;
    CREATE TABLE `rchatroom` (
      `ID` varchar(50) NOT NULL,
      `NAME` varchar(255) DEFAULT NULL,
      `DESC` varchar(255) DEFAULT NULL,
      `LABEL` varchar(255) DEFAULT NULL,
      `UID` varchar(50) NOT NULL,
      PRIMARY KEY (`ID`),
      KEY `UID` (`UID`)
    ) ENGINE=InnoDB DEFAULT CHARSET=utf8;

    @endcode

*/

#endif // TABLE_H
