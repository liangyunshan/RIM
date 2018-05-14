#include "global.h"
#include <QApplication>
#include <QAction>

QString G_TextServerIp;                         //文本服务器IP
unsigned short G_TextServerPort;                //文本服务器监听端口

QString G_FileServerIp;                         //文件服务器IP
unsigned short G_FileServerPort;                //文件服务器监听端口

//TODO
QString G_ScreenShotKeySequence;

User * G_User;                                 //当前登陆用户
OnlineStatus G_OnlineStatus;                   //在线状态
QDateTime G_loginTime;                         //登录时间

QString G_Temp_Picture_Path;
QAction *G_pScreenShotAction = NULL;
ScreenShot *G_pScreenShot = NULL;
int G_mIsEnter;

