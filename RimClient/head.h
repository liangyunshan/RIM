/*!
 *  @brief     宏定义
 *  @details   定义了软件运行时所需的宏等信息
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef HEAD_H
#define HEAD_H

#define RIM_VERSION_MAJOR    0
#define RIM_VERSION_MINOR    0
#define RIM_VERSION_PATCH    1
#define RIM_VERSION_STR      "0.0.1"

#define RIM_VERSION      RIM_VERSION_CHECK(RIM_VERSION_MAJOR, RIM_VERSION_MINOR, RIM_VERSION_PATCH)

#define RIM_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

#define RIM_USER_FILE_MAGIC  0x0A0B0C01     //用户登录信息文件魔数


#define MQ_D(Class) Class##Private * const d = dynamic_cast<Class##Private *>(d_ptr)
#define MQ_Q(class) Class * const q = q_ptr;


#endif // HEAD_H
