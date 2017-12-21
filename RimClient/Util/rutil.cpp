#include "rutil.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>
#include <QUuid>

QSettings * RUtil::gSettings = NULL;

RUtil::RUtil()
{

}

bool RUtil::createDir(QString dirPath)
{
    QFileInfo fileInfo(dirPath);
    if(!fileInfo.exists())
    {
        QDir tmpDir;
        return tmpDir.mkpath(dirPath);
    }

    return true;
}

bool RUtil::isFileExist(QString filePath)
{
    QFileInfo fileInfo(filePath);
    return fileInfo.exists();
}

QString RUtil::MD5(QString text)
{
    QByteArray bb;
    bb = QCryptographicHash::hash (text.toLocal8Bit(), QCryptographicHash::Md5 );
    return QString().append(bb.toHex());
}

QString RUtil::getTimeStamp(QString format)
{
    QDateTime dtime = QDateTime::currentDateTime();
    return dtime.toString(format);
}

QString RUtil::getDate(QString format)
{
    QDate date = QDate::currentDate();
    return date.toString(format);
}

void RUtil::setGlobalSettings(QSettings *settings)
{
    if(gSettings)
    {
        delete gSettings;
    }

    gSettings = settings;
}

QSettings *RUtil::globalSettings()
{
    return gSettings;
}

QString RUtil::UUID()
{
    return QUuid::createUuid().toString();
}
