#include "global.h"
#include <QApplication>
#include <QAction>
#include "others/serialno.h"

namespace Global {

std::queue<RecvUnit> G_TextRecvBuffs;
std::mutex G_TextRecvMutex;
std::condition_variable  G_TextRecvCondition;

std::queue<RecvUnit> G_FileRecvBuffs;
std::mutex G_FileRecvMutex;
std::condition_variable  G_FileRecvCondition;

GlobalConfigFile * G_GlobalConfigFile;              /*!< 系统配置文件解析 */

#ifdef __LOCAL_CONTACT__
ParameterSettings::ParaSettings* G_ParaSettings;   /*!< 本地参数配置 */
#endif
}


//TODO
QString G_ScreenShotKeySequence;

User * G_User;                                 //当前登陆用户
OnlineStatus G_OnlineStatus;                   //在线状态
QDateTime G_loginTime;                         //登录时间

QString G_Temp_Picture_Path;
QAction *G_pScreenShotAction = NULL;
QAction *G_pHideWindowAction = NULL;
ScreenShot *G_pScreenShot = NULL;
int G_mIsEnter = Qt::Key_Return;

unsigned short FrashSerialNo()
{
    return SerialNo::instance()->FrashSerialNo();
}

void SetSerialNo(unsigned int No)
{
    return SerialNo::instance()->SetSerialNo(No);
}
