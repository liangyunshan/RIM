#include "imagemanager.h"

#include <QDir>
#include <QApplication>
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
