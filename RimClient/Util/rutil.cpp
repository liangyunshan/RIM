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

qint64 RUtil::timeStamp()
{
    return QDateTime::currentDateTime().toMSecsSinceEpoch();
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

/*!
 * @brief 获取屏幕坐标尺寸范围
 * @details 在多窗口下，主屏和副屏有顺序关系。顺序不同，其坐标的范围存在差异 @n
 *          若A为主屏、B为辅屏，那么AB排列时，坐标范围为[0,2W][0,H] @n
 *          若BA排列时，坐标范围是[-W,W][0,H] @n
 * @warning AB、BA排列时在获取控件的位置时有较大的影响。
 * @param[in] 无
 * @return 屏幕坐标
 */
QRect RUtil::screenGeometry()
{
    int screenSize = qApp->desktop()->screenCount();

    int minX = 0,maxX = 0 ,minY = 0,maxY = 0;
    for(int i = 0; i < screenSize; i++)
    {
        QRect tmpRect = qApp->desktop()->screenGeometry(i);
        minX = tmpRect.x() < minX ? tmpRect.x() : minX;
        maxX = tmpRect.x() + tmpRect.width() > maxX ? tmpRect.x() + tmpRect.width() : maxX;
        minY = tmpRect.y() < minY ? tmpRect.y() : minY;
        maxY = tmpRect.y() + tmpRect.height() > maxY ? tmpRect.y() + tmpRect.height() : maxY;
    }

    QRect rect;
    rect.setLeft(minX);
    rect.setRight(maxX);
    rect.setTop(minY);
    rect.setBottom(maxY);

    return rect;
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

/*!
     * @brief 将图片转换成灰度图
     * @param[in] image：const QImage &，转换前的图片
     * @return 无 t_grayImage：QImage，转换后的灰度图
     */
QImage RUtil::convertToGray(const QImage & t_image)
{
    int t_height = t_image.height();
    int t_width = t_image.width();
    QImage t_grayImage(t_width, t_height, QImage::Format_Indexed8);
    t_grayImage.setColorCount(256);
    for(int i = 0; i < 256; i++)
    {
        t_grayImage.setColor(i, qRgb(i, i, i));
    }
    switch(t_image.format())
    {
    case QImage::Format_Indexed8:
        for(int i = 0; i < t_height; i ++)
        {
            const unsigned char *pSrc = (unsigned char *)t_image.constScanLine(i);
            unsigned char *pDest = (unsigned char *)t_grayImage.scanLine(i);
            memcpy(pDest, pSrc, t_width);
        }
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
        for(int i = 0; i < t_height; i ++)
        {
            const QRgb *pSrc = (QRgb *)t_image.constScanLine(i);
            unsigned char *pDest = (unsigned char *)t_grayImage.scanLine(i);

            for( int j = 0; j < t_width; j ++)
            {
                 pDest[j] = qGray(pSrc[j]);
            }
        }
        break;
    default:
        break;
    }
    return t_grayImage;
}
