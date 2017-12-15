/*!
 *  @brief     主面板底部菜单栏
 *  @details
 *  @file      panelbottomtoolbar.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.14
 *  @warning
 *  @copyright GNU Public License.
 */

#ifndef PANELBOTTOMTOOLBAR_H
#define PANELBOTTOMTOOLBAR_H

#include <QWidget>

class QToolButton;

class PanelBottomToolBar : public  QWidget
{
    Q_OBJECT
public:
    explicit PanelBottomToolBar(QWidget * parent = 0);
    ~PanelBottomToolBar();

signals:
    void showSearchPerson();

private:
    void initWidget();

private:
    QWidget * mainWidget;

    QToolButton * toolButton;
    QToolButton * searchPerson;

};

#endif // PANELBOTTOMTOOLBAR_H
