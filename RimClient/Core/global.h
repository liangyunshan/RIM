/*!
 *  @brief     全局方法或变量
 *  @details   包含全部定义的变量
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.20
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QDateTime>
#include "protocol/protocoldata.h"
#include "protocol/datastruct.h"
#include "Network/networksetting.h"

#include <queue>
#include <mutex>

class QAction;
class ScreenShot;
class User;
class GlobalConfigFile;
using namespace ProtocolType;

namespace Global {

extern std::queue<RecvUnit> G_TextRecvBuffs;
extern std::mutex G_TextRecvMutex;
extern std::condition_variable  G_TextRecvCondition;

extern std::queue<RecvUnit> G_FileRecvBuffs;
extern std::mutex G_FileRecvMutex;
extern std::condition_variable  G_FileRecvCondition;

extern GlobalConfigFile * G_GlobalConfigFile;

#ifdef __LOCAL_CONTACT__
extern ParameterSettings::ParaSettings* G_ParaSettings;   /*!< 本地参数配置 */
#endif

}

namespace ParameterSettings{
    struct ParaSettings;
}

extern QString G_Temp_Picture_Path;                     //用户登陆成功后创建的缓存文件夹
extern QString G_ScreenShotKeySequence;                 //截屏快捷按键

extern User * G_User;                                   //当前登陆用户，通过此获取当前用户的所有属性信息
extern OnlineStatus G_OnlineStatus;                     //在线状态
extern QDateTime G_loginTime;                           //登录时间

extern QAction *G_pScreenShotAction;                    //全局截屏快捷动作
extern QAction *G_pHideWindowAction;                    //设置截图时是否隐藏聊天窗口
extern ScreenShot *G_pScreenShot;                       //快捷键设置界面
extern int G_mIsEnter;

unsigned short FrashSerialNo();
void SetSerialNo(unsigned int No);

#define SERIALNO_FRASH FrashSerialNo()
#define SERIALNO_SetFrash(No) SetSerialNo(unsigned int No)

#endif // GLOBAL_H
