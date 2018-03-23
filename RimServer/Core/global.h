#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include "datastruct.h"
using namespace Datastruct;

namespace RGlobal
{
extern ServiceType G_SERVICE_TYPE;             /*!< 当前服务器运行的模式，根据不同的模式，网络数据解析存在不同 */

extern QString G_FILE_UPLOAD_PATH;             /*!< 文件上传保存的路径 */

}

#endif // GLOBAL_H
