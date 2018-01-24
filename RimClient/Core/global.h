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
#include "protocoldata.h"
using namespace ProtocolType;

extern QString G_ServerIp;                             //服务器IP
extern unsigned short G_ServerPort;                    //服务器监听端口

extern UserBaseInfo G_UserBaseInfo;                    //登陆成功后接收用户基本信息

extern QString G_Temp_Picture_Path;


#endif // GLOBAL_H
