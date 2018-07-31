/*!
 *  @brief     在线状态选择框
 *  @details   可提供在线、离线、隐身、忙碌、请勿打扰等状态显示
 *  @file      onlinestate.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.13
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note   20180224:wey:调整用户在线状态；
 */
#ifndef ONLINESTATE_H
#define ONLINESTATE_H

#include <QWidget>
#include <QMap>
#include "../protocol/protocoldata.h"

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

    void setState(const OnlineStatus & state);
    OnlineStatus state();

    static QString getStatePixmap(OnlineStatus state);

    QMenu *stateMenu() const;

signals:
    void stateChanged(OnlineStatus state);

private slots:
    void switchState(bool);

private:
    void initWidget();

private:
    OnLineStatePrivate * d_ptr;
    static QMap<OnlineStatus,QString> onlineStatePixmap;    /*!< @attention 这种方式只是声明，并未初始化。 */
};

#endif // ONLINESTATE_H
