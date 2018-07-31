#include "rutil.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDesktopWidget>
#include <QApplication>
#include <QUuid>
#include <QRegExp>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>

#include "../Core/constants.h"

#include <QDebug>

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

/*!
 * @brief 对字符串进行MD5加密
 * @param[in] text 待加密的字符串
 * @return 加密后的字符串
 */
QString RUtil::MD5(QString text)
{
    QByteArray bb;
    bb = QCryptographicHash::hash (text.toLocal8Bit(), QCryptographicHash::Md5 );
    return QString().append(bb.toHex());
}

/*!
 * @brief 获取文件的MD5信息
 * @details 对文件进行MD5计算，用于网络传输后对接收的文件数据进行正确性校验，防止传输过程中错误或者被篡改；
 * @param[in] fileName 文件名信息
 * @return 计算后的MD5值信息
 */
QString RUtil::MD5File(QString fileName)
{
    QFile theFile(fileName);
    theFile.open(QIODevice::ReadOnly);
    QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5).toHex();
    theFile.close();
    return QString(ba);
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

/*!
 * @brief 查询全局ini配置文件中字段内容
 * @param[in] group 分组名称
 * @param[in] key 待查询值key
 * @param[in] defaultValue 默认值，若group下不存在对应的key，使用默认值代替
 * @return 查询结果
 */
QVariant RUtil::getGlobalValue(const QString & group,const QString &key, const QVariant & defaultValue)
{
    gSettings->beginGroup(group);
    if(!gSettings->contains(key))
    {
        gSettings->setValue(key,defaultValue);
    }

    QVariant val = gSettings->value(key,defaultValue);

    gSettings->endGroup();

    return val;
}

/*!
 * @brief 将超长的字符串转换以...结尾的字符
 * @param[in] font 带显示区字体
 * @param[in] origin 原始显示内容
 * @param[in] maxLen 最大显示宽度
 * @return 转换后的字符
 */
QString RUtil::replaceLongTextWidthElide(const QFont &font, const QString &origin, const int maxLen)
{
    QString str;
    QFontMetrics fontWidth(font);
    if(fontWidth.width(origin) >= maxLen)
        str = fontWidth.elidedText(origin,Qt::ElideRight,maxLen);
    else
        str = origin;
    return str;
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

/*!
 * @brief 获取当前时间自格林尼治时间1970-1-1 8:0:0以来的秒数
 * @details QDateTime会先将两个时间都转换为gmt时间，再做比较。
 */
qint64 RUtil::currentSecsSinceEpoch()
{
    QDateTime epochTime(QDate(1970,1,1),QTime(8,0,0));
    return epochTime.secsTo(QDateTime::currentDateTime());
}

/*!
 * @brief RUtil::addMSecsToEpoch 将毫秒数加上格林尼治时间为实际时间
 * @param[in] mSeonds 格林尼治时间以来的毫秒数
 * @return 实际时间
 */
QDateTime RUtil::addMSecsToEpoch(qint64 mSeonds)
{
    QDateTime epochTime(QDate(1970,1,1),QTime(8,0,0));
    return epochTime.addMSecs(mSeonds);
}

/*!
 * @brief RUtil::addMSecsToEpoch 将秒数加上格林尼治时间为实际时间
 * @param[in] mSeonds 格林尼治时间以来的秒数
 * @return 实际时间
 */
QDateTime RUtil::addSecsToEpoch(qint64 seonds)
{
    QDateTime epochTime(QDate(1970,1,1),QTime(8,0,0));
    return epochTime.addSecs(seonds);
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

/*!
 * @brief RUtil::setRelativeImgPath 为目标简单的Html格式的文本框内容中img标签设置相对路径
 * @param targetHtml 需要修改img标签内容的Html格式字符串
 * @param userID 用于拼接相对路径的用户账号ID
 */
void RUtil::setRelativeImgPath(QString &targetHtml,QString userID)
{
    QDomDocument domDoc;
    domDoc.setContent(targetHtml);
    QString rootName = domDoc.documentElement().tagName();
    if(rootName == "p")
    {
        QDomNodeList nodes = domDoc.documentElement().childNodes();
        for(int index=0;index<nodes.count();index++)
        {
            if(nodes.at(index).nodeName()=="img"&&nodes.at(index).isElement())
            {
                QString t_imgPath = QString("");
                t_imgPath = t_imgPath + Constant::PATH_UserPath + QDir::separator() + userID + QDir::separator() + Constant::USER_RecvFileDirName;
                t_imgPath = t_imgPath + QDir::separator() + Constant::USER_ChatImageDirName + QDir::separator() + Constant::USER_C2CDirName;
                t_imgPath = t_imgPath + QDir::separator() + nodes.at(index).toElement().attribute("src");
                nodes.at(index).toElement().setAttribute("src",t_imgPath);
            }
        }
    }
    targetHtml = domDoc.toString();
}

/*!
 * @brief RUtil::setAbsoulteImgPath 为目标简单的Html格式的文本框内容中img标签设置绝对路径
 * @param targetHtml 需要修改img标签内容的Html格式字符串
 * @param userID 用于拼接相对路径的用户账号ID
 */

void RUtil::setAbsoulteImgPath(QString targetHtml, QString userID)
{
    QDomDocument domDoc;
    domDoc.setContent(targetHtml);
    QString rootName = domDoc.documentElement().tagName();
    if(rootName == "p")
    {
        QDomNodeList nodes = domDoc.documentElement().childNodes();
        for(int index=0;index<nodes.count();index++)
        {
            if(nodes.at(index).nodeName()=="img"&&nodes.at(index).isElement())
            {
                QString t_imgPath = QString("");
                QDir t_parentDir = QDir::current();
                t_parentDir.cdUp();
                t_imgPath = t_imgPath + t_parentDir.path() + QDir::separator();
                t_imgPath = t_imgPath + Constant::PATH_UserDirName + QDir::separator() + userID + QDir::separator() + Constant::USER_RecvFileDirName;
                t_imgPath = t_imgPath + QDir::separator() + Constant::USER_ChatImageDirName + QDir::separator() + Constant::USER_C2CDirName;
                t_imgPath = t_imgPath + QDir::separator() + nodes.at(index).toElement().attribute("src");
                t_imgPath = QDir::fromNativeSeparators(t_imgPath);
                nodes.at(index).toElement().setAttribute("src",t_imgPath);
            }
        }
    }
    targetHtml = domDoc.toString();
}

/*!
 * @brief RUtil::escapeQuote 将Html内容中的双引号、单引号进行转义
 * @param targetHtml 需要转义处理的Html内容
 */
void RUtil::escapeSingleQuote(QString &targetHtml)
{
    targetHtml = targetHtml.replace("\\","\\\\");
    targetHtml = targetHtml.replace("\'","\\\'");
    targetHtml = targetHtml.replace("\n","");
}

/*!
 * @brief RUtil::removeEccapeQuote 移除Html内容中双引号的转义符号
 * @param targetHtml 需要移除转义处理的Html内容
 */
void RUtil::removeEccapeSingleQuote(QString &targetHtml)
{
    targetHtml = targetHtml.replace("\\\'","\'");
    targetHtml = targetHtml.replace("\n","");
}

/*!
 * @brief RUtil::escapeDoubleQuote
 * @param targetHtml
 */
void RUtil::escapeDoubleQuote(QString &targetHtml)
{
    targetHtml = targetHtml.replace("\"","\\\"");
    targetHtml = targetHtml.replace("\n","");
}

/*!
 * @brief RUtil::removeEccapeDoubleQuote
 * @param targetHtml
 */
void RUtil::removeEccapeDoubleQuote(QString &targetHtml)
{
    targetHtml = targetHtml.replace("\\\"","\"");
    targetHtml = targetHtml.replace("\n","");
}
