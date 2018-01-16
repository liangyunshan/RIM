/*!
 *  @brief     主面板内容区
 *  @details   显示联系人、群组、历史聊天对象等内容
 *  @file      panelcontentarea.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef PANELCONTENTAREA_H
#define PANELCONTENTAREA_H

#include <QWidget>
#include <QTabBar>
#include "observer.h"

class PanelContentAreaPrivate;
class QStackedWidget;
class QButtonGroup;
class QResizeEvent;
class QToolButton;
class QHBoxLayout;
class QLabel;

class PanelTabItem : public QWidget
{
    Q_OBJECT
public:
    explicit PanelTabItem(QWidget * parent = 0);

    bool isSelected();
    void setSelected(bool flag);

    void setTextVisible(bool flag = false);

    void setText(const QString & text);
    QString text()const;

signals:
    void selectedMe();

protected:
    void mousePressEvent(QMouseEvent *);

private:
    QWidget * contentWidget;
    QLabel * iconLabel;
    QLabel * textLabel;

    bool selected;
};

class PanelTabBar : public QWidget
{
    Q_OBJECT
public:
    explicit PanelTabBar(QWidget * parent = 0);

    PanelTabItem *addTab();
    void removeTab(int index);
    void setCurrentIndex(int index);
    int currentIndex();
    int count(){return this->tabItems.size();}

signals:
    void currentIndexChanged(int);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void switchItemIndex();

private:
    QWidget * contentWidget;
    QList<PanelTabItem *> tabItems;

    QHBoxLayout * contentLayout;

    int itemSelectedIndex;
};

class PanelPersonPage;
class PanelHistoryPage;
class PanelGroupPage;

class PanelContentArea : public QWidget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelContentArea)
public:
    explicit PanelContentArea(QWidget * parent = 0);
    ~PanelContentArea();

    void onMessage(MessageType type);

private:
    void initWidget();

private:
    PanelContentAreaPrivate * d_ptr;
};

#endif // PANELCONTENTAREA_H
