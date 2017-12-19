/*!
 *  @brief     常量汇总
 *  @details   定义了软件运行时所需的常量信息，包括(图片、控件ID等)
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constant {

/*图片*/
const char ICON_SYSTEM24[] = ":/icon/resource/icon/icon_system_24.png";
const char ICON_LOKC24[] = ":/icon/resource/icon/icon_lock.png";
const char ICON_SIGN24[] = ":/icon/resource/icon/icon_sign.png";
const char ICON_PANEL_SEARCH[] = ":/icon/resource/icon/panel_search.png";


/*文件路径*/
const char ApplicationName[] = "RimClient";
const char PATH_UserPath[] = "/../users";
const char PATH_ImagePath[] = "/../image";
const char PATH_ConfigPath[] = "/../config";
const char PATH_LocalePath[] = "/translations";
const char PATH_StylePath[] = "/style";
const char PATH_SystemIconPath[] = "/systemIcon";

/*控件尺寸*/
const int TOOL_WIDTH = 24;                              //工具按钮宽度
const int TOOL_HEIGHT = 24;                             //工具按钮高度

const int MAIN_PANEL_MIN_WIDTH = 300;                   //主面板最小宽度
const int MAIN_PANEL_MIN_HEIGHT = 450;                  //主面板最小高度

const int MAIN_PANEL_MAX_WIDTH = 600;                   //主面板最大宽度

/*! @brief  ToolButton ID
    @details 在设置控件的名称时，采用[类型_位置_功能描述]:
             如类型为QToolButton，其处于主面板Panel,用于添加联系人AddPerson，则最终的变量名命名为TOOL_PANEL_ADDPERSON;对应的值为Tool_Panel_AddPerson；
             若控件属于全局，采用[类型_功能_类型]:TOOL_MIN  Tool_Min_Button

*/
const char TOOL_MIN[] = "Tool_Min_Button";
const char TOOL_CLOSE[] = "Tool_Close_Button";
const char TOOL_SETTING[] = "Tool_Setting_Button";

const char TOOL_PANEL_FRONT[] = "Tool_Panel_Front";
const char TOOL_PANEL_TOOL[] = "Tool_Panel_Tool";
const char TOOL_PANEL_ADDPERSON[] = "Tool_Panel_AddPerson";


const char MENU_PANEL_TOOLBOX[] = "Menu.Panel.ToolBox";
const char MENU_PANEL_TOOLGROUP[] = "Menu.Panel.ToolGroup";
const char MENU_PANEL_TOOLITEM_PERSON[] = "Menu.Panel.ToolItem.Person";


const char ACTION_PANEL_REFRESH[] = "Action_Panel_Refresh";
const char ACTION_PANEL_ADDGROUP[] = "Action_Panel_AddGroup";
const char ACTION_PANEL_RENAME[] = "Action_Panel_Rename";
const char ACTION_PANEL_DELGROUP[] = "Action_Panel_DelGroup";

const char ACTION_PANEL_SENDMESSAGE[] = "Action_Panel_SendPersonMessage";
const char ACTION_PANEL_VIEWDETAIL[] = "Action_Panel_ViewDetail";
const char ACTION_PANEL_MODIFYCOMMENTS[] = "Action_Panel_ModifyComments";
const char ACTION_PANEL_DELPERSON[] = "Action_Panel_DeltePerson";

}

#endif // CONSTANTS_H
