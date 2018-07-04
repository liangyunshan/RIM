#include "json_msgparse.h"

#ifndef __LOCAL_CONTACT__

#include <QJsonDocument>
#include <memory>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "jsonkey.h"
#include "global.h"
#include "../msgprocess/json_dataprocess.h"

using namespace ProtocolType;

Json_MsgParse::Json_MsgParse()
{

}

/*!
 * @brief 分发数据类型
 * @param[in] toolButton 待插入的工具按钮
 * @return 是否插入成功
 */
void Json_MsgParse::processData(const ProtocolPackage &recvData)
{
    QJsonDocument document = QJsonDocument::fromJson(recvData.data,&jsonParseError);
    if(jsonParseError.error == QJsonParseError::NoError)
    {
        QJsonObject root = document.object();

        switch(root.value(JsonKey::key(JsonKey::Type)).toInt())
        {
            case MSG_CONTROL:
                handleCommandMsg((MsgCommand)root.value(JsonKey::key(JsonKey::Command)).toInt(),root);
                break;
            case MSG_TEXT:
                handleTextMsg((MsgCommand)root.value(JsonKey::key(JsonKey::Command)).toInt(),root);
                break;
            default:
                break;
        }
    }
}

/*!
 * @brief 处理命令消息
 * @param[in] commandType 命令类型
 * @param[in] obj 数据主体
 * @return 无
 */
void Json_MsgParse::handleCommandMsg(MsgCommand commandType, QJsonObject &obj)
{
    switch(commandType)
    {
        case MSG_USER_REGISTER:
                RSingleton<Json_DataProcess>::instance()->proRegistResponse(obj);
                break;
        case MSG_USER_LOGIN:
                RSingleton<Json_DataProcess>::instance()->proLoginResponse(obj);
                break;
        case MSG_USER_UPDATE_INFO:
                RSingleton<Json_DataProcess>::instance()->proUpdateBaseInfoResponse(obj);
                break;
        case MSG_USER_STATE:
                RSingleton<Json_DataProcess>::instance()->proUserStateChanged(obj);
                break;
        case MSG_RELATION_SEARCH:
                RSingleton<Json_DataProcess>::instance()->proSearchFriendResponse(obj);
                break;
        case MSG_REALTION_ADD:
                RSingleton<Json_DataProcess>::instance()->proAddFriendResponse(obj);
                break;
        case MSG_RELATION_OPERATE:
                RSingleton<Json_DataProcess>::instance()->proOperateFriendResponse(obj);
                break;
        case MSG_RELATION_LIST:
                RSingleton<Json_DataProcess>::instance()->proFriendListResponse(obj);
                break;
        case MSG_GROUPING_OPERATE:
                RSingleton<Json_DataProcess>::instance()->proGroupingOperateResponse(obj);
                break;
        case MSG_RELATION_GROUPING_FRIEND:
                RSingleton<Json_DataProcess>::instance()->proGroupingFriendResponse(obj);
                break;
        case MSG_GROUP_LIST:
                RSingleton<Json_DataProcess>::instance()->proGroupListResponse(obj);
                break;
        case MSG_GROUP_CREATE:
                RSingleton<Json_DataProcess>::instance()->proRegistGroupResponse(obj);
                break;
        case MSG_GROUP_SEARCH:
                RSingleton<Json_DataProcess>::instance()->proSearchGroupResponse(obj);
                break;
        case MSG_GROUP_OPERATE:
                RSingleton<Json_DataProcess>::instance()->proOpreateGroupResponse(obj);
                break;
        case MSG_GROUP_COMMAND:
                RSingleton<Json_DataProcess>::instance()->proGroupCommandResponse(obj);
                break;
        default:break;
    };
}

void Json_MsgParse::handleTextMsg(MsgCommand commandType, QJsonObject &obj)
{
    switch(commandType)
    {
        case MSG_TEXT_TEXT:
        case MSG_TEXT_SHAKE:
        case MSG_TEXT_AUDIO:
        case MSG_TEXT_FILE:
        case MSG_TEXT_IMAGE:
            RSingleton<Json_DataProcess>::instance()->proText(obj);
            break;

        case MSG_TEXT_APPLY:
            RSingleton<Json_DataProcess>::instance()->proTextApply(obj);
            break;

        default:
            break;
    }
}

#endif
