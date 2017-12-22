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

#include <QFileInfoList>
#include <QIcon>

class ImageManager
{
public:
    ImageManager();

    void loadSystemIcons();

    /*!
     * @brief 获取用户图标
     *
     * @param[in] index 图片索引
     * @param[in] fullPath 是否以全路径返回
     *
     * @return 若存在则返回，若不存在返回空字符串
     */
    QString getSystemUserIcon(int index = 0,bool fullPath = true);

    /*!
     * @brief 获取用户图标
     *
     * @param[in] imageName 文件名
     *
     * @return 若存在则返回全路径，若不存在返回空字符串
     */
    QString getSystemUserIcon(QString imageName);

    QIcon getCircularIcons(QString imagePath);

private:
    QFileInfoList localSystemUserIcon;
};

#endif // IMAGEMANAGER_H
