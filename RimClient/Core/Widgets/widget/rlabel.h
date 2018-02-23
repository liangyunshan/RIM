/*!
 *  @brief     自定义圆角label
 *  @details   实现绘制圆角图形
 *  @file      riconlabel.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.21
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RICONLABEL_H
#define RICONLABEL_H

#include <QLabel>
#include <QFileInfo>

class RIconLabelPrivate;

class RTextLabel : public QLabel
{
    Q_OBJECT
public:
    explicit RTextLabel(QWidget * parent  = 0);

signals:
    void mousePress();

protected:
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void mouseReleaseEvent(QMouseEvent *);
};

class RIconLabel : public QLabel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RIconLabel)
public:
    explicit RIconLabel(QWidget * parent = 0);

    void setCorner(bool isCorner = true);
    void setBackroundColor(QColor color);
    void setTransparency(bool flag = false);
    void setEnterCursorChanged(bool flag = true);
    void setHoverDelay(bool flag = true);
    void setEnterHighlight(bool flag = false);
    void setEnterHighlightColor(QColor color);

    void setPixmap(const QString &fileName);    
    void setPixmap(const QPixmap &);

    QFileInfo getPixmapFileInfo();

    void resizeByPixmap(bool flag = true);

protected:
    void paintEvent(QPaintEvent * event);
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void timeOut();

signals:
    void mouseHover(bool flag);
    void mousePressed();
    void mouseReleased();

private:
    RIconLabelPrivate * d_ptr;

};

#endif // RICONLABEL_H
