/*!
 *  @brief     正则验证
 *  @details   提供通用的正则表达式验证
 *  @file      regexp.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.02.06
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef REGEXP_H
#define REGEXP_H

#include <QObject>
#include <QMap>

#include "util_global.h"

class UTILSHARED_EXPORT RegExp
{
public:
    RegExp();

    enum Type
    {
        FULL_SPACE,         //全部空格
        START_SPACE,        //以空格开头
        END_SPACE,          //以空格结尾
        IP,                 //IP地址
        PHONE_NUM,          //手机号11位
        MAIL                //邮箱
    };

    bool getValidate(Type type,QString originText);

    QString regexp(Type type);

private:
    QMap<Type,QString> regexps;

};

#endif // REGEXP_H
