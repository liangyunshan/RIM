/*!
 *  @brief     数据处理中转
 *  @details   处理接收数据，并将结果分发至对应的页面
 *  @file      dataprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.17
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QJsonObject>

class DataProcess
{
public:
    DataProcess();

    void proRegistResponse(QJsonObject &data);
    void proLoginResponse(QJsonObject &data);

private:

};

#endif // DATAPROCESS_H
