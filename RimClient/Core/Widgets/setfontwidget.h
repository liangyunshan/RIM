/*!
 *  @brief     自定义聊天窗口中字体设置窗口
 *  @details   允许设置字体、字号、加粗、斜体、颜色
 *  @file      setfontwidget.h
 *  @author    LYS
 *  @version   1.0
 *  @date      2018.05.11
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      20180511:LYS:未完成
 */
#ifndef SETFONTWIDGET_H
#define SETFONTWIDGET_H

#include <QWidget>

class SetFontWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SetFontWidget(QWidget *parent = 0);

signals:

public slots:
};

#endif // SETFONTWIDGET_H
