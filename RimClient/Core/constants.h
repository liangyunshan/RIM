/*!
 *  @brief     常量汇总
 *  @details   定义了软件运行时所需的常量信息，包括(图片、控件ID等)
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 *  @change:
 *      date:20180122   content:添加ACTION_PANEL_MOVEPERSON     name:LYS
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constant {

/*日期*/
const char Date_Simple[] = "yyyy-MM-dd";

/*正则表达式*/
const char AccountId_Reg[] = "[1-9]\\d{4}";
const char AccountPassword_Reg[] = "\\w{1,16}";
const char Space_Reg[] = "\\s+";
const char SingleIp_Reg[] = "(\\d|([1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))";
const char FullIp_Reg[] = "(%1\\.){3}%2";

/*文件路径*/
const char AppShortName[] = "Rim";
const char ApplicationName[] = "RimClient";
const char UserTempName[] = "temp";
const char UserDBDirName[] = "db";
const char UserDBFileName[] = "rimclient.db";
const char PATH_UserPath[] = "/../users";

const char PATH_ImagePath[] = "/../image";
const char PATH_SystemIconPath[] = "/systemIcon";

const char PATH_ConfigPath[] = "/../config";
const char PATH_LocalePath[] = "/translations";
const char PATH_StylePath[] = "/style";
const char PATH_SountPath[] = "/sound";


/*系统设置*/
const char SETTING_X[] = "Main/X";
const char SETTING_Y[] = "Main/Y";
const char SETTING_WIDTH[] = "Main/Width";
const char SETTING_HEIGHT[] = "Main/Height";

const char SETTING_AUTO_STARTUP[] = "Main/MainAutoStartUp";
const char SETTING_AUTO_LOGIN[] = "Main/AutoLogin";

const char SETTING_TOPHINT[] = "Main/TopHint";
const char SETTING_EXIT_SYSTEM[] = "Main/ExitSystem";
const char SETTING_TRAYICON[] = "Main/TrayIcon";

const char SETTING_WINDOW_SHAKE[] = "Main/WindowShake";
const char SETTING_SOUND_AVAILABLE[] = "Main/Sounds";

const char SETTING_SYSTEM_LOCK[] = "Main/SystemLock";
const char SETTING_EXIT_DELRECORD[] = "Main/ExitDelRecord";

const char SETTING_NETWORK_IP[] = "Network/LoginIp";
const char SETTING_NETWORK_PORT[] = "Network/LoginPort";

const char SETTING_SOUND_GROUP[] = "Sound";

const QString DEFAULT_NETWORK_IP = "127.0.0.1";
const QString DEFAULT_NETWORK_PORT = "8023";

/*图片*/
const char ICON_LOKC24[] = ":/icon/resource/icon/icon_lock.png";
const char ICON_SIGN24[] = ":/icon/resource/icon/icon_sign.png";
const char ICON_PANEL_SEARCH[] = ":/icon/resource/icon/panel_search.png";


/*控件尺寸*/
const int TOOL_BAR_HEIGHT = 30;                         //工具栏固定高度
const int TOOL_WIDTH = 30;                              //工具按钮宽度
const int TOOL_HEIGHT = 30;                             //工具按钮高度

const int MAIN_PANEL_MIN_WIDTH = 300;                   //主面板最小宽度
const int MAIN_PANEL_MIN_HEIGHT = 450;                  //主面板最小高度

const int MAIN_PANEL_MAX_WIDTH = 600;                   //主面板最大宽度

const int LOGIN_FIX_WIDTH = 430;                        //登陆页面固定宽度
const int LOGIN_FIX_HEIGHT =  350;                      //登陆页面固定高度

const int ITEM_FIX_HEIGHT = 24;                         //QLineEdit、QLabel等的固定高度

const int ICON_USER_SIZE =  40;                         //用户头像固定尺寸

/*! @brief  ToolButton ID
    @details 在设置控件的名称时，采用[类型_位置_功能描述]:
             如类型为QToolButton，其处于主面板Panel,用于添加联系人AddPerson，则最终的变量名命名为TOOL_PANEL_ADDPERSON;对应的值为Tool_Panel_AddPerson；
             若控件属于全局，采用[类型_功能_类型]:TOOL_MIN  Tool_Min_Button

*/

const char TOOL_MIN[] = "Tool_Min_Button";
const char TOOL_MAX[] = "Tool_Max_Button_True";
const char TOOL_CLOSE[] = "Tool_Close_Button";
const char TOOL_SETTING[] = "Tool_Setting_Button";

const char TOOL_PANEL_FRONT[] = "Tool_Panel_Front";
const char TOOL_PANEL_TOOL[] = "Tool_Panel_Tool";
const char TOOL_PANEL_ADDPERSON[] = "Tool_Panel_AddPerson";
const char TOOL_PANEL_NOTIFY[] = "Tool_Panel_Notify";

const char Tool_Chat_Call[] = "Tool_Chat_Call";
const char Tool_Chat_Min[] = "Tool_Chat_Min";
const char Tool_Chat_Close[] = "Tool_Chat_Close";

const char Tool_Chat_Font[] = "Tool_Chat_Font";                                                 //字体
const char Tool_Chat_FontColor[] = "Tool_Chat_FontColor";                                       //字体颜色
const char Tool_Chat_Face[] = "Tool_Chat_Face";                                                 //表情
const char Tool_Chat_Shake[] = "Tool_Chat_Shake";                                               //窗口抖动
const char Tool_Chat_Image[] = "Tool_Chat_Image";                                               //图像
const char Tool_Chat_ScreenShot[] = "Tool_Chat_ScreenShot";                                     //截图
const char Tool_Chat_Record[] = "Tool_Chat_Record";                                             //聊天记录

const char Tool_Chat_SendMess[] = "Tool_Chat_SendMess";                                         //聊天自动回复

const char Button_Chat_Close_Window[] = "Button_Chat_Close_Window";                             //聊天窗口关闭按钮
const char Button_Chat_Send[] = "Button_Chat_Send";                                             //聊天发送按钮

const char MENU_PANEL_PERSON_TOOLBOX[] = "Menu.Panel.Person.ToolBox";
const char MENU_PANEL_PERSON_TOOLGROUP[] = "Menu.Panel.Person.ToolGroup";
const char MENU_PANEL_PERSON_TOOLITEM[] = "Menu.Panel.Person.ToolItem";
const char MENU_PANEL_PERSON_TOOLITEM_GROUPS[] = "Menu.Panel.Person.ToolItem.Groups";
const char MENU_PANEL_GROUP_TOOLITEM_GROUPS[] = "Menu.Panel.Group.ToolItem.Groups";

const char ACTION_PANEL_PERSON_REFRESH[] = "Action_Panel_Person_Refresh";
const char ACTION_PANEL_PERSON_ADDGROUP[] = "Action_Panel_Person_AddGroup";
const char ACTION_PANEL_PERSON_RENAME[] = "Action_Panel_Person_Rename";
const char ACTION_PANEL_PERSON_DELGROUP[] = "Action_Panel_Person_DelGroup";

const char ACTION_PANEL_SENDMESSAGE[] = "Action_Panel_SendPersonMessage";
const char ACTION_PANEL_VIEWDETAIL[] = "Action_Panel_ViewDetail";
const char ACTION_PANEL_MODIFYCOMMENTS[] = "Action_Panel_ModifyComments";
const char ACTION_PANEL_DELPERSON[] = "Action_Panel_DeltePerson";
const char ACTION_PANEL_MOVEPERSON[] = "Action_Panel_MovePerson";                               //移动联系人至

const char MENU_PANEL_GROUP_TOOLBOX[] = "Menu.Panel.Group.ToolBox";
const char MENU_PANEL_GROUP_TOOLGROUP[] = "Menu.Panel.Group.ToolGroup";
const char MENU_PANEL_GROUP_TOOLITEM[] = "Menu.Panel.Group.ToolItem";

const char ACTION_PANEL_GROUP_SEARCHGROUP[] = "Action_Panel_Group_SearchGroup";                 //查找群
const char ACTION_PANEL_GROUP_ADDGROUP[] = "Action_Panel_Group_AddGroup";                       //添加群
const char ACTION_PANEL_GROUP_ADDGROUPS[] = "Action_Panel_Group_AddGroups";                     //添加群分组

const char ACTION_PANEL_GROUP_RENAME[] = "Action_Panel_Group_Rename";                           //重命名分组
const char ACTION_PANEL_GROUP_DELGROUP[] = "Action_Panel_Group_DelGroup";                       //删除分组
const char ACTION_PANEL_GROUP_MOVEGROUPTO[] = "Action_Panel_Group_MovGroup";                       //删除分组

const char ACTION_PANEL_GROUP_SENDMESS[] = "Action_Panel_Group_SenMessage";                     //发送群消息
const char ACTION_PANEL_GROUP_VIEWDETAIL[] = "Action_Panel_Group_ViewDetail";                   //查看群资料
const char ACTION_PANEL_GROUP_MODIFYCOMMENTS[] = "Action_Panel_Group_ModifyComments";           //修改备注名称
const char ACTION_PANEL_GROUP_EXITGROUP[] = "Action_Panel_Group_ExitGroup";                     //退出该群

const char MENU_PANEL_HISTORY_TOOLBOX[] = "Menu.Panel.History.ToolBox";
const char MENU_PANEL_HISTORY_TOOLITEM[] = "Menu.Panel.History.ToolItem";

const char ACTION_PANEL_HISTORY_CLEARLIST[] = "Action_Panel_History_ClearList";                 //清空会话列表

const char ACTION_PANEL_HISTORY_STICKSESSION[] = "Action_Panel_Group_StickSession";             //会话置顶
const char ACTION_PANEL_HISTORY_REMOVEFORMLIST[] = "Action_Panel_Group_RemoveFromList";         //从列表中移除

const char MENU_CHAT_SCREEN_SHOT[] = "Menu.Chat.Screen.Shot";                                   //截图菜单

const char ACTION_CHAT_SCREEN_SHOT[] = "Action_Chat_Screen_Shot";                               //截图
const char ACTION_CHAT_SCREEN_HIDEWIDOW[] = "Action_Chat_Screen_HideWindow";                    //截图隐藏当前窗口

}

#endif // CONSTANTS_H
