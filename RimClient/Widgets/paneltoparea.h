/*!
 *  @brief     主面板顶部信息区
 *  @details   显示登录用户的头像、名称、签名并支持以后扩展添加工具
 *  @file      paneltoparea.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef PANELTOPAREA_H
#define PANELTOPAREA_H

#include <QWidget>
#include "observer.h"

class QLabel;
class QLineEdit;
class PanelTopAreaPrivate;

class PanelTopArea : public QWidget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelTopArea)
public:
    explicit PanelTopArea(QWidget *parent = 0);
    ~PanelTopArea();

    void onMessage(MessageType);

signals:

private:
    PanelTopAreaPrivate * d_ptr;
};

#endif // PANELTOPAREA_H
