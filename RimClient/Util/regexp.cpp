#include "regexp.h"

#include <QRegExp>

RegExp::RegExp()
{
    regexps.insert(FULL_SPACE,"\\s+");
    regexps.insert(PHONE_NUM,"\\d{11}");
    regexps.insert(MAIL,"^[0-9a-zA-Z]+@[0-9a-zA-Z]+(\\.[0-9a-zA-Z]+)+$");

    QString SingleIp_Reg = "(\\d|([1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))";
    regexps.insert(IP,QString("(%1\\.){3}%2").arg(SingleIp_Reg,SingleIp_Reg));
}

bool RegExp::getValidate(RegExp::Type type, QString originText)
{
    QRegExp exp;
    exp.setPattern(regexps.value(type));
    return exp.exactMatch(originText);
}

QString RegExp::regexp(RegExp::Type type)
{
    return regexps.value(type);
}
