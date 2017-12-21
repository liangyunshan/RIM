/*!
 *  @brief     通用工具类
 *  @details   定义通用的函数工具
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef RUTIL_H
#define RUTIL_H

#include <QString>

#include <QSettings>

class RUtil
{
public:
    RUtil();

    static bool createDir(QString dirPath);
    static bool isFileExist(QString filePath);

    static QString MD5(QString text);

    static QString getTimeStamp(QString format = "yyyy-MM-dd hh:mm:ss:zzz");
    static QString getDate(QString format = "yyyy-MM-dd");

    static void setGlobalSettings(QSettings * settings);
    static QSettings * globalSettings();

    static QString UUID();

private:
    static QSettings * gSettings;
};

#endif // RUTIL_H
