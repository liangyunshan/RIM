#include "customfontfamilydelegate.h"

#include <QFont>
#include <QEvent>
#include <QPainter>
#include <QFontInfo>
#include <QFontDatabase>

#include <QDebug>

static QFontDatabase::WritingSystem writingSystemForFont(const QFont &font, bool *hasLatin)
{
    *hasLatin = true;

    QList<QFontDatabase::WritingSystem> writingSystems = QFontDatabase().writingSystems(font.family());

    // this just confuses the algorithm below. Vietnamese is Latin with lots of special chars
    writingSystems.removeAll(QFontDatabase::Vietnamese);

    QFontDatabase::WritingSystem system = QFontDatabase::Any;

    if (!writingSystems.contains(QFontDatabase::Latin))
    {
        *hasLatin = false;
        // we need to show something
        if (writingSystems.count())
            system = writingSystems.last();
    }
    else
    {
        writingSystems.removeAll(QFontDatabase::Latin);
    }

    if (writingSystems.isEmpty())
        return system;

    if (writingSystems.count() == 1 && writingSystems.at(0) > QFontDatabase::Cyrillic)
    {
        system = writingSystems.at(0);
        return system;
    }

    if (writingSystems.count() <= 2
        && writingSystems.last() > QFontDatabase::Armenian
        && writingSystems.last() < QFontDatabase::Vietnamese) {
        system = writingSystems.last();
        return system;
    }

    if (writingSystems.count() <= 5
        && writingSystems.last() >= QFontDatabase::SimplifiedChinese
        && writingSystems.last() <= QFontDatabase::Korean)
        system = writingSystems.last();

    return system;
}

CustomFontFamilyDelegate::CustomFontFamilyDelegate(QObject *parent) :
    QItemDelegate(parent)
{
    writingSystem = QFontDatabase::Any;
}

CustomFontFamilyDelegate::~CustomFontFamilyDelegate()
{

}

void CustomFontFamilyDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFont font(option.font);
    font.setPointSize(QFontInfo(font).pointSize() * 3 / 2);
    QFont font2 = font;
    font2.setFamily(text);

    bool hasLatin;
    QFontDatabase::WritingSystem system = writingSystemForFont(font2, &hasLatin);
    if(hasLatin)
        font = font2;

    QRect r = option.rect;

    if(option.state & QStyle::State_Selected)
    {
        painter->save();
        painter->setBrush(option.palette.highlight());
        painter->setPen(Qt::NoPen);
        painter->drawRect(option.rect);
        painter->setPen(QPen(option.palette.highlightedText(), 0));
    }

    if(option.direction == Qt::RightToLeft)
        r.setRight(r.right() - 4);
    else
        r.setLeft(r.left() + 4);

    QFont old = painter->font();
    painter->setFont(font);

    QFontMetricsF fontMetrics(font);
    int h = fontMetrics.height();
    painter->drawText(r.x(), r.y()+h/4, r.width(), h, Qt::AlignTop|Qt::AlignLeading|Qt::TextSingleLine, text);

    if(writingSystem != QFontDatabase::Any)
        system = writingSystem;

    if(system != QFontDatabase::Any)
    {
        QFont font3 = font;
        font2.setFamily(text);
        font3.setPointSize(font.pointSize()-5);
        painter->setFont(font3);
        QString sample = QFontDatabase().writingSystemSample(system);
        painter->drawText(r.x(), r.y()+h*5/4, r.width(), h, Qt::AlignVCenter|Qt::AlignLeading|Qt::TextSingleLine, sample);
    }
    painter->setFont(old);

    if(option.state & QStyle::State_Selected)
        painter->restore();
}

QSize CustomFontFamilyDelegate::sizeHint(const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFont font(option.font);
    font.setPointSize(QFontInfo(font).pointSize() * 3/2);
    QFontMetrics fontMetrics(font);
    return QSize(fontMetrics.width(text), fontMetrics.height()*5/2);
}
