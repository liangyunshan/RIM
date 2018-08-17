#include "globalconfigfile.h"

#include "Util/rutil.h"
#include "constants.h"

GlobalConfigFile::GlobalConfigFile():settings(nullptr)
{

}

bool GlobalConfigFile::parseFile()
{
    if(settings == nullptr)
        return false;

    //解析日志模块
    logConfig.isRecord2File = RUtil::getGlobalValue(Constant::LOG_LOG,Constant::LOG_LOG_RECORDFILE,true).toBool();
    logConfig.level = RUtil::getGlobalValue(Constant::LOG_LOG,Constant::LOG_LOG_LEVEL,RLog::RINFO).toInt();

    return true;
}
