/*!
 *  @brief     主面板内容区
 *  @details   显示联系人、群组、历史聊天对象等内容
 *  @file      panelcontentarea.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef PANELCONTENTAREA_H
#define PANELCONTENTAREA_H

#include <QWidget>
#include <QTabBar>

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

signals:
    void selectedMe();

protected:
    void mousePressEvent(QMouseEvent * event);

private:
    QWidget * contentWidget;
    QLabel * label;

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

    bool itemSelectedIndex;
};

class PanelPersonPage;
class PanelHistoryPage;
class PanelGroupPage;

class PanelContentArea : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelContentArea)
public:
    explicit PanelContentArea(QWidget * parent = 0);
    ~PanelContentArea();

private:
    void initWidget();

private:
    PanelContentAreaPrivate * d_ptr;

    QWidget * mainWidget;

    PanelTabBar * panelTabBar;
    QStackedWidget * panelStackedWidget;

    PanelPersonPage * personWidget;
    PanelHistoryPage * historyWidget;
    PanelGroupPage * groupWidget;

};

#endif // PANELCONTENTAREA_H
