/*!
 *  @brief     路由信息操作
 *  @details   用户快速复制信息
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.21
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef ROUTESETTINGS_H
#define ROUTESETTINGS_H

#include "../protocol/datastruct.h"

class RouteSettings
{
public:
    explicit RouteSettings();
    ~RouteSettings();
    RouteSettings(const RouteSettings & origin);

    RouteSettings & operator=(const RouteSettings & origin);

    void initSettings(ParameterSettings::RouteSettings * setts);

    ParameterSettings::RouteSettings * settings();

private:
    void release();
    void copy(ParameterSettings::RouteSettings *setts);

private:
    ParameterSettings::RouteSettings * rsettings;

};

#endif // ROUTESETTINGS_H
