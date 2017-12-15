/*!
 *  @brief     在线状态选择框
 *  @details   可提供在线、离线、隐身、忙碌、请勿打扰等状态显示
 *  @file      onlinestate.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.13
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef ONLINESTATE_H
#define ONLINESTATE_H

#include <QWidget>

class QToolButton;
class QAction;
class QMenu;

class OnLineStatePrivate;

class OnLineState : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OnLineState)

public:
    explicit OnLineState(QWidget * parent = 0);
    ~OnLineState();

public:
    /*!
     * @brief 用户在线状态
     */
    enum UserState
    {
        STATE_ONLINE,       /*!< 在线 */
        STATE_OFFLINE,      /*!< 离开 */
        STATE_BUSY,         /*!< 忙碌 */
        STATE_HIDE,         /*!< 隐身 */
        STATE_NODISTURB     /*!< 请勿打扰 */
    };
    Q_FLAG(UserState)

    void setState(const UserState & state);
    UserState state();

signals:
    void stateChanged(UserState state);

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
