#include "imagemanager.h"

#include <QDir>
#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QMetaEnum>
#include <QDebug>

#include "constants.h"

ImageManager::ImageManager()
{

}

void ImageManager::loadSystemIcons()
{
   localSystemUserIcon.clear();

   QDir dir(qApp->applicationDirPath()+Constant::PATH_ImagePath+Constant::PATH_SystemIconPath);
   if(dir.exists())
   {
       localSystemUserIcon = dir.entryInfoList(QStringList(QObject::tr("*.png")),QDir::Files|QDir::NoDotAndDotDot);
   }
}

QString ImageManager::getSystemUserIcon(int index, bool fullPath)
{
    if(localSystemUserIcon.size() == 0)
    {
        loadSystemIcons();
    }

    if(index >=0 && index < localSystemUserIcon.size())
    {
        if(fullPath)
        {
            return localSystemUserIcon.at(index).absoluteFilePath();
        }
        return localSystemUserIcon.at(index).fileName();
    }

    return QString("");
}

QString ImageManager::getSystemUserIcon(QString imageName)
{
    if(localSystemUserIcon.size() == 0)
    {
        loadSystemIcons();
    }

    if(localSystemUserIcon.size() > 0)
    {
        foreach(QFileInfo info,localSystemUserIcon)
        {
            if(info.fileName() == imageName)
            {
                return info.absoluteFilePath();
            }
        }
    }

    return QString("");
}

QIcon ImageManager::getCircularIcons(QString imagePath)
{
    QPixmap pixmap(imagePath);
    if(pixmap.isNull())
    {
        return QIcon();
    }

    QPixmap result(pixmap.size());
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setBackgroundMode(Qt::TransparentMode);
    QPainterPath path;
    path.addEllipse(0, 0, pixmap.width(), pixmap.height());
    painter.setClipPath(path);
    painter.drawPixmap(0,0,pixmap);

    return QIcon(result);
}

QString ImageManager::getWindowIcon(WinIconColor color,WinIcon icon)
{
#if QT_VERSION > 0x050500
    QMetaEnum iconMetaEnum =  QMetaEnum::fromType<WinIcon>();
    QMetaEnum colorMetaEnum =  QMetaEnum::fromType<WinIconColor>();

    qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<QString(":/icon/resource/icon/%1_%2.png").arg(QString(iconMetaEnum.key(icon)).toLower(),QString(colorMetaEnum.key(color)).toLower());
    return QString(":/icon/resource/icon/%1_%2.png").arg(QString(iconMetaEnum.key(icon)).toLower(),QString(colorMetaEnum.key(color)).toLower());
#else

#endif

}
