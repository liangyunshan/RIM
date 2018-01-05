/*!
 *  @brief     图像管理
 *  @details   用于加载系统存在的图片资源，提供对外访问接口
 *  @file      imagemanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.12
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QFileInfoList>
#include <QIcon>

#include "datastruct.h"

class ImageManager : public QObject
{
    Q_OBJECT
public:
    ImageManager();

    /*!
     *  @brief 系统图标尺寸 w*h pix
     */
    enum WinIcon
    {
        ICON_SYSTEM_16,                 //用于窗口工具栏左上角
        ICON_SYSTEM_24,                 //用于windows中的任务栏、系统托盘
        ICON_SYSTEM_32,
        ICON_SYSTEM_48,
        ICON_SYSTEM_64,
        ICON_SYSTEM_72,
        ICON_SYSTEM_96,
        ICON_SYSTEM_128
    };
    Q_ENUM(WinIcon)

    enum WinIconColor
    {
        NORMAL,                         //浅色背景使用
        WHITE                           //深色背景使用
    };

    Q_ENUM(WinIconColor)

    void loadSystemIcons();
    const QFileInfoList systemIcons();

    QString getSystemUserIcon(int index = 0,bool fullPath = true);

    QString getSystemUserIcon(QString imageName);

    QString getSystemImageDir();

    QIcon getCircularIcons(QString imagePath);

    QString getWindowIcon(WinIconColor color = NORMAL, WinIcon icon = ICON_SYSTEM_24);

private:
    QFileInfoList localSystemUserIcon;
};

#endif // IMAGEMANAGER_H
