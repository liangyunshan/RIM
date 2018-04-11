/*!
 *  @brief     图像管理
 *  @details   用于加载系统存在的图片资源，提供对外访问接口
 *  @file      imagemanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.12
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20180127:wey:将图片的名称和尺寸分开设置
 */
#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QFileInfoList>
#include <QIcon>

#include "util_global.h"
#include "Core/datastruct.h"

class UTILSHARED_EXPORT ImageManager : public QObject
{
    Q_OBJECT
public:
    ImageManager();

    /*!
     *  @brief 图标尺寸
     */
    enum IconSize
    {
        ICON_16 = 16,                   //用于窗口工具栏左上角
        ICON_24 = 24,                   //用于windows中的任务栏、系统托盘
        ICON_32 = 32,
        ICON_48 = 48,
        ICON_64 = 64,                   //用户头像
        ICON_72 = 72,
        ICON_96 = 96,
        ICON_128 = 128,
    };
    Q_ENUM(IconSize)

    /*!
     *  @brief 图标类型
     */
    enum IconType
    {
        ICON_SYSTEM,
        ICON_SYSTEMNOTIFY,
        ICON_SUCCESS,
        ICON_STAR,
        ICON_ERROR
    };
    Q_ENUM(IconType)

    enum WinIconColor
    {
        NORMAL,                         //浅色背景使用
        WHITE                           //深色背景使用
    };

    Q_ENUM(WinIconColor)

    void loadSystemIcons();
    const QFileInfoList systemIcons();

    QString getSystemUserIcon(unsigned short index = 1, bool fullPath = true);
    QString getSystemUserIcon(QString imageName);
    QString getSystemImageDir();

    QIcon getCircularIcons(QString imagePath);

    QString getIcon(IconType type = ICON_SYSTEM,IconSize size = ICON_24);
    QString getWindowIcon(WinIconColor color = NORMAL,IconType type = ICON_SYSTEM,IconSize size = ICON_24);

private:
    QFileInfoList localSystemUserIcon;
};

#endif // IMAGEMANAGER_H
