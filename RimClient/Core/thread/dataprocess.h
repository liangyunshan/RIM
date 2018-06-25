/*!
 *  @brief     数据处理中转
 *  @details   处理接收数据，并将结果分发至对应的页面
 *  @file      dataprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.17
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note   20180124:wey:增加更新个人基本信息
 */
#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <QJsonObject>
#include "Util/rbuffer.h"

class DataProcess
{
public:
    DataProcess();

    void proRegistResponse(QJsonObject &data);
    void proLoginResponse(QJsonObject &data);
    void proUpdateBaseInfoResponse(QJsonObject &data);
    void proUserStateChanged(QJsonObject &data);

    void proSearchFriendResponse(QJsonObject &data);
    void proAddFriendResponse(QJsonObject &data);
    void proOperateFriendResponse(QJsonObject &data);
    void proFriendListResponse(QJsonObject &data);
    void proGroupingOperateResponse(QJsonObject &data);
    void proGroupingFriendResponse(QJsonObject &data);

    void proGroupListResponse(QJsonObject & data);
    void proRegistGroupResponse(QJsonObject & data);
    void proSearchGroupResponse(QJsonObject & data);
    void proOpreateGroupResponse(QJsonObject & data);
    void proGroupCommandResponse(QJsonObject & data);

    void proText(QJsonObject &data);
    void proTextApply(QJsonObject &data);

    void proFileControl(RBuffer &data);
    void proFileRequest(RBuffer & data);
    void proFileData(RBuffer & data);

    void proTCPText(RBuffer &data);

private:

};

#endif // DATAPROCESS_H
