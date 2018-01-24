/*!
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
#include <QProcess>

class AbstractChatWidget : public Widget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractChatWidget)
public:
    //窗口抖动的次数
    enum
    {MaxLimitTimes = 12};

    //窗口抖动的幅度
    enum
    {MaxLimitSpace = 5};
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
    void slot_ShakeWidget(bool flag);
    void slot_ShakeTimeout();
    void slot_ScreenShot(bool flag);
    void slot_ScreenShot_Ready(int finish, QProcess::ExitStatus exitStatus);
    void slot_ScreenShotHide(bool flag);
    void slot_ScreenTimeout();
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
    QProcess *p_shotProcess;
    bool b_isScreeHide;
    QTimer *p_shotTimer;
    QTimer *p_shakeTimer;
    int     m_nPosition;
    QPoint  m_curPos;
};

#endif // ABSTRACTCHATWIDGET_H
