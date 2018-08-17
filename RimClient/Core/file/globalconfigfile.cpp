#include "globalconfigfile.h"

#include "../constants.h"
#include "Util/rutil.h"

GlobalConfigFile::GlobalConfigFile():settings(nullptr),
    defaultMaxTransFileSize(10),
    defaultMaxTransPacketLength(512)
{

}

bool GlobalConfigFile::parseFile()
{
    if(settings == nullptr)
        return false;

    //解析网络
    netSettings.textServer.ip = RUtil::getGlobalValue(Constant::SYSTEM_NETWORK,Constant::SYSTEM_NETWORK_TEXT_IP,Constant::DEFAULT_NETWORK_TEXT_IP).toString();
    netSettings.textServer.port = RUtil::getGlobalValue(Constant::SYSTEM_NETWORK,Constant::SYSTEM_NETWORK_TEXT_PORT,Constant::DEFAULT_NETWORK_TEXT_PORT).toUInt();

#ifdef __LOCAL_CONTACT__
    netSettings.tandemServer.ip = RUtil::getGlobalValue(Constant::SYSTEM_NETWORK,Constant::SYSTEM_NETWORK_TANDEM_IP1,Constant::DEFAULT_NETWORK_TEXT_IP).toString();
    netSettings.tandemServer.port = RUtil::getGlobalValue(Constant::SYSTEM_NETWORK,Constant::SYSTEM_NETWORK_TANDEM_PORT1,"").toUInt();
#else
    netSettings.fileServer.ip = RUtil::getGlobalValue(Constant::SYSTEM_NETWORK,Constant::SYSTEM_NETWORK_FILE_IP,Constant::DEFAULT_NETWORK_FILE_IP).toString();
    netSettings.fileServer.port = RUtil::getGlobalValue(Constant::SYSTEM_NETWORK,Constant::SYSTEM_NETWORK_FILE_PORT,Constant::DEFAULT_NETWORK_FILE_PORT).toUInt();
#endif

    //解析数据库
    databaseConfigInfo.type = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_TYPE,Constant::DEFAULT_SQL_TYPE).toString();
    databaseConfigInfo.hostName =RUtil:: getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_HOSTNAME,Constant::DEFAULT_SQL_HOST).toString();
    databaseConfigInfo.databaseName = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_DATABASE,Constant::DEFAULT_SQL_DATABASE).toString();
    databaseConfigInfo.userName =RUtil:: getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_USER,Constant::DEFAULT_SQL_USER).toString();
    databaseConfigInfo.port = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_PORT,Constant::DEFAULT_SQL_PORT).toString().toInt();
    databaseConfigInfo.useInnerPass = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_INNER_PASSWORD,Constant::DEFAULT_SQL_INNER_PASS).toBool();
    databaseConfigInfo.password = RUtil::getGlobalValue(Constant::SYSTEM_DB,Constant::SYSTEM_DB_PASS,"").toString();

    //解析日志模块
    logConfig.isRecord2File = RUtil::getGlobalValue(Constant::LOG_LOG,Constant::LOG_LOG_RECORDFILE,true).toBool();
    logConfig.level = RUtil::getGlobalValue(Constant::LOG_LOG,Constant::LOG_LOG_LEVEL,RLog::RINFO).toInt();

    //系统模块
    systemSetting.isTrayIconVisible = RUtil::getGlobalValue(Constant::SETTING_GERNALE,Constant::SETTING_TRAYICON,true).toBool();

    //网络传输设置
    transSetting.maxTransFileOnce = RUtil::getGlobalValue(Constant::TRANS_SETTING,Constant::TRANS_SETTING_MAX_FILE,defaultMaxTransFileSize).toInt();
    if(transSetting.maxTransFileOnce <= 0 || transSetting.maxTransFileOnce > defaultMaxTransFileSize)
        transSetting.maxTransFileOnce = defaultMaxTransFileSize;

    transSetting.maxTransLength = RUtil::getGlobalValue(Constant::TRANS_SETTING,Constant::TRANS_SETTING_MAX_PACKET,defaultMaxTransPacketLength).toInt();
    if(transSetting.maxTransLength <= 64 || transSetting.maxTransLength > 64*1024)
        transSetting.maxTransLength = defaultMaxTransPacketLength;

    return true;
}

/*!
 * @brief 将修改的信息更新至磁盘
 */
void GlobalConfigFile::sysc()
{
    if(settings == nullptr)
        return;

    settings->beginGroup(Constant::SYSTEM_NETWORK);
    settings->setValue(Constant::SYSTEM_NETWORK_TEXT_IP,netSettings.textServer.ip);
    settings->setValue(Constant::SYSTEM_NETWORK_TEXT_PORT,netSettings.textServer.port);

#ifdef __LOCAL_CONTACT__
    settings->setValue(Constant::SYSTEM_NETWORK_TANDEM_IP1,netSettings.tandemServer.ip);
    settings->setValue(Constant::SYSTEM_NETWORK_TANDEM_PORT1,netSettings.tandemServer.port);
#else
    settings->setValue(Constant::SYSTEM_NETWORK_FILE_IP,netSettings.fileServer.ip);
    settings->setValue(Constant::SYSTEM_NETWORK_FILE_PORT,netSettings.fileServer.port);
#endif
    settings->endGroup();

    settings->beginGroup(Constant::SETTING_GERNALE);
    settings->setValue(Constant::SETTING_TRAYICON,systemSetting.isTrayIconVisible);
    settings->endGroup();
}
