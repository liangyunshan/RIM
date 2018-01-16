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

class MsgContext
{
public:
    MsgContext();

    const QString msgType;
    const QString msgCommand;
    const QString msgData;
    const QString msgStatus;
};

#endif // MSGCONTEXT_H
