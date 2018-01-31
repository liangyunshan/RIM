#include "global.h"
#include <QApplication>
#include <QAction>

QString G_ServerIp;                             //服务器IP
unsigned short G_ServerPort;                    //服务器监听端口

//TODO
QString G_ScreenShotKeySequence;

UserBaseInfo G_UserBaseInfo;                   //登陆成功后接收用户基本信息
QList<RGroupData *> G_FriendList;              //保存登陆用户的好友列表信息

QString G_Temp_Picture_Path;
QAction *G_pScreenShotAction = NULL;
ScreenShot *G_pScreenShot = NULL;
int G_mIsEnter;

