/*!
 *  @brief     JSON字段标识
 *  @details   描述JSON各个key的信息
 *  @file      msgcontext.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSGCONTEXT_H
#define MSGCONTEXT_H

#include <QObject>

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

#endif // MSGCONTEXT_H
