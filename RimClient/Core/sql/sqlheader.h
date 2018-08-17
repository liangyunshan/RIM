#ifndef SQLHEADER_H
#define SQLHEADER_H

#include <QString>

namespace SQLHeader {

/*!
 *  @brief 数据配置信息
 *  @details 描述一种数据库配置信息
 */
struct DatabaseConfigInfo
{
    QString type;               /*!< 数据库类型 */
    QString hostName;           /*!< 主机名 */
    QString databaseName;       /*!< 数据库名 */
    unsigned short port;        /*!< 端口号 */
    bool useInnerPass;          /*!< 是否使用程序内置的密码 */
    QString userName;           /*!< 用户名 */
    QString password;           /*!< 密码 */
};

}

#endif // SQLHEADER_H
