/*!
 *  @brief     参数配置解析页面
 *  @details   解析config/参数配置.txt文件
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.04
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef XMLPARSE_H
#define XMLPARSE_H

#include <QObject>

namespace ParameterSettings
{
    struct ParaSettings;
}

class XMLParse : public QObject
{
    Q_OBJECT
public:
    explicit XMLParse(QObject * parent = nullptr);
    ~XMLParse();

    bool parse(const QString & fileName, ParameterSettings::ParaSettings *paraSettings);

};

#endif // XMLPARSE_H
