/*!
 *  @brief     单例模板类
 *  @details   快速提供单例子模板
 *  @file      rsingleton.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef RSINGLETON_H
#define RSINGLETON_H

#include <QObject>

template<class T>
class RSingleton
{
public:
    static T * instance()
    {
        static T * t = NULL;
        if(!t)
        {
            t = new T();
        }
        return t;
    }

};

#endif // RSINGLETON_H
