/*!
 *  @brief     群信息界面
 *  @details   显示登录用户群信息
 *  @file      panelgrouppage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef PANELGROUPPAGE_H
#define PANELGROUPPAGE_H

#include <QWidget>

class PanelGroupPagePrivate;

class PanelGroupPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelGroupPage)
public:
    explicit PanelGroupPage(QWidget *parent = 0);
    ~PanelGroupPage();

private:
    void initWidget();

private:
    PanelGroupPagePrivate * d_ptr;

    QWidget * contentWidget;

};

#endif // PANELGROUPPAGE_H
