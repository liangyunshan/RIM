/*!
 *  @brief     宏定义
 *  @details   定义了软件运行时所需的宏等信息
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef HEAD_H
#define HEAD_H

#include <QDebug>

#define RIM_VERSION_MAJOR    0
#define RIM_VERSION_MINOR    0
#define RIM_VERSION_PATCH    1
#define RIM_VERSION_STR      "0.0.1"

#define RIM_VERSION      RIM_VERSION_CHECK(RIM_VERSION_MAJOR, RIM_VERSION_MINOR, RIM_VERSION_PATCH)

#define RIM_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define RIM_USER_FILE_MAGIC  0x0A0B0C01     //用户登录信息文件魔数


#define MQ_D(Class) Class##Private * const d = dynamic_cast<Class##Private *>(d_ptr)
#define MQ_Q(class) Class * const q = q_ptr;

/*在线检测，主动检测，
 *[1]在发送消息前进行网络状态检测，会存在G_User->isTextOnLine()返回true，但服务器已经关闭，此时会触发send发送返回结果为-1，
 *客户端会更新G_User->setTextOnline(false)，主动设置为离线。这样避免在离线状态下多次进行数据操作。
 *[2]在第一次登陆、用户注册、第一次发送好友列表、第一次发送群列表请求时，无需调用此函数验证；
 *[3]【若为在线状态，下拉列表改变执行状态更新操作；若为离线状态，下拉列表切换执行重连操作】；
*/
#define R_CHECK_ONLINE if(!G_User->isTextOnLine()) return

#endif // HEAD_H
