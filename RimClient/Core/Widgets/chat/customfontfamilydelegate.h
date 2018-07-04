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
