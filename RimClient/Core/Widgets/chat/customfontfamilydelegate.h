/*!
 *  @brief     自定义字体选择下拉框代理
 *  @details   代理显示某个路径下已有字体文件中可用字体
 *  @file      customfontfamilydelegate.h
 *  @author    LYS
 *  @version   1.0.2
 *  @date      2018.06.12
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      1.20180612-LYS 修复显示信息不完整问题。
 *
 */
#ifndef CUSTOMFONTFAMILYDELEGATE_H
#define CUSTOMFONTFAMILYDELEGATE_H

#include <QItemDelegate>

#include <QFontDatabase>

class CustomFontFamilyDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CustomFontFamilyDelegate(QObject *parent = Q_NULLPTR);
    ~CustomFontFamilyDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

    QFontDatabase::WritingSystem writingSystem;
};

#endif // CUSTOMFONTFAMILYDELEGATE_H
