/*!
 *  @brief     订阅端
 *  @details   订阅消息，根据消息类型，及时刷新显示
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef OBSERVER_H
#define OBSERVER_H

#include "datastruct.h"
#include "subject.h"

class Observer
{
public:
    Observer();
    ~Observer();

    virtual void onMessage(MessageType type) = 0;
};

#endif // OBSERVER_H
