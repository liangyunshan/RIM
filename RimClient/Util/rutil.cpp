#include "rutil.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QApplication>
#include <QUuid>
#include <QRegExp>

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
    QString uuid = QUuid::createUuid().toString();
    uuid.replace(QRegExp("\\W+"),"");
    return uuid;
}

QSize RUtil::screenSize(int num)
{
    if(num >= 0 && num < qApp->desktop()->screenCount())
    {
        return qApp->desktop()->screen(num)->size();
    }

    return qApp->desktop()->screen()->size();
}

qint64 RUtil::currentMSecsSinceEpoch()
{
    return QDateTime::currentDateTime().currentMSecsSinceEpoch();
}

bool RUtil::validateIpFormat(QString dest)
{
    QString matchIp = "(\\d|([1-9]\\d)|(1\\d{2})|(2[0-4]\\d)|(25[0-5]))";
    QString matchWholeIp = QString("(%1\\.){3}%2").arg(matchIp).arg(matchIp);
    QRegExp rx(matchWholeIp);

    return rx.exactMatch(dest);
}
