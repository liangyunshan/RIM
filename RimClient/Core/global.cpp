#include "global.h"
#include <QApplication>

QString G_ServerIp;                             //服务器IP
unsigned short G_ServerPort;                    //服务器监听端口

QString G_Temp_Picture_Path = qApp->applicationDirPath() + "/temp";
QString G_ScreenShotKeySequence = "Ctrl+Alt+A";
