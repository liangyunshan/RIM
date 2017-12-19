/*!
 *  @brief     面板子页面
 *  @details   对应主页面中不同功能页面。
 *  @file      panelcontentpage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.18
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef PANELCONTENTPAGE_H
#define PANELCONTENTPAGE_H

#include <QWidget>

class PanelContentPagePrivate;

class PanelContentPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelContentPage)
public:
    explicit PanelContentPage(QWidget *parent = 0);
    ~PanelContentPage();

    void setContentWidget(QWidget * wid);

protected:
    void initWidget();

private:
    PanelContentPagePrivate * d_ptr;

};

#endif // PANELCONTENTPAGE_H
