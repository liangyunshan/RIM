/*!
 *  @brief     类型定义
 *  @details   定义了通用的结构体、枚举等变量
 *  @file      datastruct.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.02
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATASTRUCT_H
#define DATASTRUCT_H

#include <QString>

namespace Datastruct {

/*!
 *  @brief 系统支持的服务类型
 *  @details 用户以命令行方式输入对应的服务.
 */
enum ServiceType
{
    SERVICE_TEXT,               /*!< 使用传输文本服务,默认类型 */
    SERVICE_FILE                /*!< 使用传输文件服务 */
};

/*!
 *  @brief 传输模式
 */
enum TransmissionMode
{
    TRANS_NET,              /*!< 网络传输方式 */
    TRANS_TEL               /*!< 电话传输方式 */
};

/*!
 *  @brief 数据库类型
 */
enum DatabaseType
{
    DB_MYSQL,               /*!< 使用MySql数据库*/
    DB_ORACLE               /*!< 使用Oracle数据库*/
};

/*!
 * @brief 解析结果
 */
enum OperateType
{
    PARSE_ERROR,            /*!< 发生错误*/
    VIEW_PROGRAM,           /*!< 查看程序信息*/
    EXEC_PROGRAM            /*!< 执行程序*/
};

struct CommandParameter
{
    CommandParameter()
    {
        serviceType = SERVICE_TEXT;
        transMode = TRANS_NET;
        dbType = DB_MYSQL;
        parseResult = PARSE_ERROR;
    }

    ServiceType serviceType;
    TransmissionMode  transMode;
    DatabaseType dbType;
    OperateType parseResult;
};

struct FileItemInfo
{
    QString fileName;               /*!< 文件名称 @attention 维护文件真实的信息 */
    size_t size;                    /*!< 文件大小 */
    QString fileId;                 /*!< 文件数据库中唯一标识 */
    QString md5;                    /*!< 文件MD5 @attention 服务器以此文件名作为在磁盘中保存的索引 */
    QString filePath;               /*!< 文件保存的路径 */
    QString accountId;              /*!< 发送方的ID */
    QString otherId;                /*!< 接收方ID */
};

} //namespace Datastruct

#endif // DATASTRUCT_H
