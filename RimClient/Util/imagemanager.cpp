#include "imagemanager.h"

#include <QDir>
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>

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
