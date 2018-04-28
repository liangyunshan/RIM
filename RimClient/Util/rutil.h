/*!
 *  @brief     通用工具类
 *  @details   定义通用的函数工具
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.11
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20180210:LYS:添加正常图像转换为灰度图方法convertToGray
 */
#ifndef RUTIL_H
#define RUTIL_H

#include <QString>
#include <QSettings>

#include "util_global.h"

class UTILSHARED_EXPORT RUtil
{
public:
    RUtil();

    static bool createDir(QString dirPath);
    static bool isFileExist(QString filePath);

    static QString MD5(QString text);
    static QString MD5File(QString fileName);

    static QString getTimeStamp(QString format = "yyyy-MM-dd hh:mm:ss:zzz");
    static QString getDate(QString format = "yyyy-MM-dd");
    static qint64 timeStamp();

    static void setGlobalSettings(QSettings * settings);
    static QSettings * globalSettings();
    static QVariant getGlobalValue(const QString & group, const QString &key, const QVariant &defaultValue);

    static QString UUID();

    static QSize screenSize(int num = -1);

    static QRect screenGeometry();

    static qint64 currentMSecsSinceEpoch();
    static qint64 currentSecsSinceEpoch();

    static bool validateIpFormat(QString dest);

    static QImage convertToGray(const QImage &);

    static void setRelativeImgPath(QString &targetHtml,QString userID);

    static void escapeQuote(QString &targetHtml);

    static void removeEccapeQuote(QString &targetHtml);

private:
    static QSettings * gSettings;
};

#endif // RUTIL_H
