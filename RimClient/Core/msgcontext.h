/*!
 *  @brief     上下文信息
 *  @details   定义程序中使用到的常量，可解决硬编码
 *  @file      msgcontext.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.17
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGCONTEXT_H
#define MSGCONTEXT_H

#include <QString>

//网络请求时JSON字符中key值
class MsgRequestContext
{
public:
    MsgRequestContext();
    const QString msgType;
    const QString msgCommand;
    const QString msgData;
};

class MsgResponseContext
{
public:
    MsgResponseContext();

    const QString msgType;
    const QString msgCommand;
    const QString msgStatus;
    const QString msgData;
};

//网络数据解析JSON字段文件中各个字含义
struct JContext
{
    JContext();
    const QString magicNum;
    const QString messType;
    const QString commandType;
    const QString timestamp;
    const QString messLength;
    const QString content;
};

#endif // MSGCONTEXT_H
