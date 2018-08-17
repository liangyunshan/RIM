/*!
 *  @brief     软件启动配置文件
 *  @details   启动时解析/config/config.ini文件
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.17
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef GLOBALCONFIGFILE_H
#define GLOBALCONFIGFILE_H

#include "../network/networksetting.h"
#include "../sql/sqlheader.h"
#include "Util/rlog.h"

#include <QSettings>

class GlobalConfigFile
{
public:
    GlobalConfigFile();

    /*!
     *  @brief 系统设置参数
     */
    struct SystemSetting{
        bool isTrayIconVisible;             /*!< 系统托盘是否可见 */
    };

    struct TransSetting{
        unsigned short maxTransFileOnce;    /*!< 一次最大发送文件数量 */
        int maxTransLength;                 /*!< 一次数据包最大的字节长度(495后的内容) */
    };

    void setSettings(QSettings * setting){this->settings = setting; }
    bool parseFile();

    void sysc();

public:
    NetworkSettings netSettings;
    SQLHeader::DatabaseConfigInfo databaseConfigInfo;
    RLog::LogConfig logConfig;
    SystemSetting systemSetting;
    TransSetting transSetting;

private:
    QSettings * settings;
    int defaultMaxTransFileSize;
    int defaultMaxTransPacketLength;

};

#endif // GLOBALCONFIGFILE_H
