/*!
 *  @brief     日志记录
 *  @details   可记录不同级别的信息，包括出错的文件、行号、错误信息等。
 *  @file      rlog.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.13
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef RLOG_H
#define RLOG_H

#define LOG_INFO(...)       RLog::log(RLog::INFO,__FILE__,__FUNCTION__,__LINE__, __VA_ARGS__)
#define LOG_WARNING(...)    RLog::log(RLog::WARNING, __FILE__,__FUNCTION__, __LINE__,__VA_ARGS__)
#define LOG_ERROR(...)      RLog::log(RLog::ERROR,__FILE__, __FUNCTION__,__LINE__, __VA_ARGS__)

#include <QFile>
#include <QMutex>

class RLog : public QObject
{
    Q_OBJECT
public:

    /*!
     *  @brief 日志等级
     */
    enum RLOG_LEVEL
    {
        INFO,               /*!<    提示  */
        WARNING,            /*!<    警告  */
        ERROR               /*!<    提示  */
    };
    Q_FLAG(RLOG_LEVEL)

    RLog();

    void setLogLevel(const RLog::RLOG_LEVEL & level);
    RLog::RLOG_LEVEL getLogLevel(){return logLevel;}

    static bool init();

    static void log(RLOG_LEVEL nLevel,const char * fileDesc,const char * functionDesc, int lineNum,const char* data, ...);

private:
    static QString getLeveDesc(RLOG_LEVEL level);

private:
    static bool isRecord2File;
    static bool isFileReady;
    static RLOG_LEVEL logLevel;
};

#endif // RLOG_H
