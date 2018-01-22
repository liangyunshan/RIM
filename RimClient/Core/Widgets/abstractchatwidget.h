﻿/*!
 *  @brief     聊天窗口
 *  @details   抽象了个人、群聊窗口的特点
 *  @file      abstractchatwidget.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.20
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef ABSTRACTCHATWIDGET_H
#define ABSTRACTCHATWIDGET_H

#include "widget.h"
#include "observer.h"

class AbstractChatWidgetPrivate;
class DatabaseThread;

class AbstractChatWidget : public Widget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractChatWidget)
public:
    AbstractChatWidget(QWidget * parent = 0);
    ~AbstractChatWidget();

    QString widgetId();
    void recvChatMsg(QByteArray);

    void onMessage(MessageType type);

private slots:
    void resizeOnce();
    void setSideVisible(bool flag);
    //shangchao
    void slot_SetChatEditFont(bool flag);
    void slot_SetChatEditFontColor(bool flag);
    void slot_ButtClick_SendMsg(bool flag);
    void slot_DatabaseThread_ResultReady(int,TextUnit::ChatInfoUnitList);
    //

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void resizeEvent(QResizeEvent * event);

private:
    void switchWindowSize();
    //shangchao
    void initChatRecord();
    //

private:
    AbstractChatWidgetPrivate * d_ptr;
    DatabaseThread * p_DatabaseThread;
};

#endif // ABSTRACTCHATWIDGET_H
