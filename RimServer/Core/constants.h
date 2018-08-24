/*!
 *  @brief     常量汇总
 *  @details   定义了软件运行时所需的常量信息，包括(图片、控件ID等)
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constant {

/*日期*/
const char Date_Simple[] = "yyyy-MM-dd";

/*文件路径*/
const char AppShortName[] = "Rim";
const char ApplicationName[] = "RimServer";
const char Version[] = "1.1.0";
const char PATH_UserPath[] = "/../users";
const char PATH_ImagePath[] = "/../image";
const char PATH_ConfigPath[] = "/../config";
const char PATH_File[] = "/../file";
const char PATH_LocalePath[] = "/translations";
const char PATH_StylePath[] = "/style";
const char PATH_SystemIconPath[] = "/systemIcon";
const char CONFIG_LocalePath[] = "/translations";

/*系统设置*/
const char GroupNetwork[] = "Network";
const char DB_THREAD[] = "MessageRecvProcCount";
const char MSG_THREAD[] = "MessageSendProcCount";
const char TEXT_PORT[] = "TextServicePort";
const char FILE_PORT[] = "FileServicePort";
const char TEXT_IP[] = "TextServiceIp";
const char FILE_IP[] = "FileServiceIp";

//日志模块
const char LOG_LOG[] = "log";
const char LOG_LOG_RECORDFILE[] = "log2File";
const char LOG_LOG_LEVEL[] = "logLevel";

const char FileServerSetting[] = "FileServer";
const char UPLOAD_FILE_PATH [] = "UploadFilePath";

const char TRANS_SETTING[] = "TransSetting";
const char BROADCAST_DEST_NODE[] = "BroadcastNodes";


const char SELECTED_LINE_COLOR[] = "rgb(34, 175, 75)";              /*!< 选中行的背景颜色 */

const char LOCAL_ROUTE_CONFIG_FILE[] = "LocalRouteConfigFile";                 /*!< 本地路由配置表 */

/*! @brief  ToolButton ID
    @details 在设置控件的名称时，采用[类型_位置_功能描述]:
             如类型为QToolButton，其处于主面板Panel,用于添加联系人AddPerson，则最终的变量名命名为TOOL_PANEL_ADDPERSON;对应的值为Tool_Panel_AddPerson；
             若控件属于全局，采用[类型_功能_类型]:TOOL_MIN  Tool_Min_Button

*/
const char MENU_ROUTE_TABLE_SERVER[] = "Menu.Route.Table.Server";             //路由表中服务器右键菜单

const char ACTION_ROUTE_SERVER_NEW[] = "Action_Route_Server_New";             //新建分组
const char ACTION_ROUTE_SERVER_DELETE[] = "Action_Route_Server_Delete";       //删除分组

const char MENU_ROUTE_TABLE_CLIENT[] = "Menu.Route.Table.Client";             //路由表中客户端右键菜单

const char ACTION_ROUTE_CLIENT_NEW[] = "Action_Route_Client_New";             //新建分组
const char ACTION_ROUTE_CLIENT_MOVE[] = "Action_Route_Client_Move";           //移动分组
const char ACTION_ROUTE_CLIENT_DELETE[] = "Action_Route_Clinet_Delete";       //删除分组

}

#endif // CONSTANTS_H
