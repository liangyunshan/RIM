#include "imagemanager.h"

#include <QDir>
#include <QApplication>
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QMetaEnum>
#include <QDebug>

#include "Core/constants.h"

ImageManager::ImageManager()
{

}

/*!
 * @brief 加载程序目录的图片，保存其基本信息
 * @param[in] 无
 * @return 无
 */
void ImageManager::loadSystemIcons()
{
   localSystemUserIcon.clear();

   QDir dir(qApp->applicationDirPath()+Constant::PATH_ImagePath+Constant::IMAGE_SystemIconPath);
   if(dir.exists())
   {
       localSystemUserIcon = dir.entryInfoList(QStringList(QObject::tr("*.png")),QDir::Files|QDir::NoDotAndDotDot,QDir::Name);
       std::sort(localSystemUserIcon.begin(),localSystemUserIcon.end(),[](const QFileInfo & a,const QFileInfo & b){
           return a.baseName().toInt() < b.baseName().toInt();
       });
   }
}

const QFileInfoList ImageManager::systemIcons()
{
    return localSystemUserIcon;
}

/*!
 * @brief 获取用户图标
 * @param[in] index 图片索引
 * @param[in] fullPath 是否以全路径返回
 * @return 若存在则返回，若不存在返回空字符串
 */
QString ImageManager::getSystemUserIcon(unsigned short index, bool fullPath)
{
    if(localSystemUserIcon.size() == 0)
    {
        loadSystemIcons();
    }

    if(index >=1 && index <= localSystemUserIcon.size())
    {
        if(fullPath)
        {
            return localSystemUserIcon.at(index - 1).absoluteFilePath();
        }
        return localSystemUserIcon.at(index - 1).fileName();
    }

    return QString("");
}

/*!
 * @brief 获取用户图标
 * @param[in] imageName 文件名(1.png)
 * @return 若存在则返回全路径，若不存在返回空字符串
 */
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

QString ImageManager::getSystemImageDir()
{
    QDir dir(qApp->applicationDirPath()+Constant::PATH_ImagePath+Constant::IMAGE_SystemIconPath);
    return dir.absolutePath();
}

/*!
 * @brief 获取圆形图标
 * @param[in] imagePath 图片路径
 * @return 处理后圆形图标
 */
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

/*!
 * @brief 根据尺寸获取对应系统图标
 * @param[in] type 图表类型
 * @param[in] size 图标尺寸
 * @return 图片路径
 */
QString ImageManager::getIcon(ImageManager::IconType type, ImageManager::IconSize size)
{
    QMetaEnum iconMetaEnum =  QMetaEnum::fromType<IconType>();

    return QString(":/icon/resource/icon/%1_%2.png").arg(QString(iconMetaEnum.key(type)).toLower(),QString::number((int)size));
}

/*!
 * @brief 根据尺寸获取对应系统图标
 * @param[in] WinIcon 图标尺寸
 * @return 图标资源路径
 */
QString ImageManager::getWindowIcon(WinIconColor color,IconType type,IconSize size)
{
    QMetaEnum iconMetaEnum =  QMetaEnum::fromType<IconType>();
    QMetaEnum colorMetaEnum =  QMetaEnum::fromType<WinIconColor>();

    return QString(":/icon/resource/icon/%1_%2_%3.png").arg(QString(iconMetaEnum.key(type)).toLower(),QString::number((int)size),
                                                            QString(colorMetaEnum.key(color)).toLower());
}
