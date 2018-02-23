/*!
 *  @brief     在线状态选择框
 *  @details   可提供在线、离线、隐身、忙碌、请勿打扰等状态显示
 *  @file      onlinestate.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.13
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef ONLINESTATE_H
#define ONLINESTATE_H

#include <QWidget>
#include "../protocoldata.h"

class QToolButton;
class QAction;
class QMenu;

class OnLineStatePrivate;

using namespace ProtocolType;

class OnLineState : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnLineState)

public:
    explicit OnLineState(QWidget * parent = 0);
    ~OnLineState();

public:
    void setState(const OnlineStatus & state);
    OnlineStatus state();

signals:
    void stateChanged(OnlineStatus state);

private slots:
    void switchState(bool);

private:
    void initWidget();

private:
    QToolButton * mainButton;
    QMenu * popMenu;

    OnLineStatePrivate * d_ptr;
    friend class OnLineStatePrivate;
};

#endif // ONLINESTATE_H
