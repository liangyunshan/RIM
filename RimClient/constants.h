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

/*文件路径*/
const char ApplicationName[] = "RimClient";
const char PATH_UserPath[] = "/../users";
const char PATH_ImagePath[] = "/../image";
const char PATH_ConfigPath[] = "/../config";
const char PATH_LocalePath[] = "/translations";
const char PATH_StylePath[] = "/style";
const char PATH_SystemIconPath[] = "/systemIcon";

/*控件ID*/
const int TOOL_WIDTH = 24;
const int TOOL_HEIGHT = 24;

const int MAIN_PANEL_MIN_WIDTH = 300;
const int MAIN_PANEL_MIN_HEIGHT = 450;

const int MAIN_PANEL_MAX_WIDTH = 600;

const char TOOL_MIN[] = "Min_TOOL_Button";
const char TOOL_CLOSE[] = "Close_TOOL_Button";
const char TOOL_SETTING[] = "Setting_TOOL_Button";
const char TOOL_SSG[] = "TOOL_SSG";

}

#endif // CONSTANTS_H
