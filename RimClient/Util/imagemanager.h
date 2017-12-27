/*!
 *  @brief     图像管理
 *  @details   用于加载系统存在的图片资源，提供对外访问接口
 *  @file      imagemanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.12
 *  @warning
 *  @copyright GNU Public License.
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

    /*!
     * @brief 获取用户图标
     * @param[in] index 图片索引
     * @param[in] fullPath 是否以全路径返回
     * @return 若存在则返回，若不存在返回空字符串
     */
    QString getSystemUserIcon(int index = 0,bool fullPath = true);

    /*!
     * @brief 获取用户图标
     * @param[in] imageName 文件名
     * @return 若存在则返回全路径，若不存在返回空字符串
     */
    QString getSystemUserIcon(QString imageName);

    QIcon getCircularIcons(QString imagePath);

    /*!
     * @brief 根据尺寸获取对应系统图标
     * @param[in] WinIcon 图标尺寸
     * @return 图标资源路径
     */
    QString getWindowIcon(WinIconColor color = NORMAL, WinIcon icon = ICON_SYSTEM_24);

private:
    QFileInfoList localSystemUserIcon;
};

#endif // IMAGEMANAGER_H
