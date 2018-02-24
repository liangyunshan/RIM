#include "onlinestate.h"

#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QMetaEnum>
#include <QHBoxLayout>

#include "datastruct.h"
#include "head.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "constants.h"

class OnLineStatePrivate : public QObject,public GlobalData<OnLineState>
{
    Q_DECLARE_PUBLIC(OnLineState)
public:
    OnLineStatePrivate()
    {
        isInitPixmap = false;
    }
    ~OnLineStatePrivate(){}

    void setState(const OnlineStatus state)
    {
        this->currState = state;
    }

    OnlineStatus state()
    {
        return this->currState;
    }

private:
    OnlineStatus currState;

    QMap<OnlineStatus,QString> stateFilePaths;
    QMap<OnlineStatus,QString> stateTips;

    QToolButton * mainButton;
    QMenu * popMenu;

    bool isInitPixmap;
};

OnLineState::OnLineState(QWidget *parent):
    d_ptr(new OnLineStatePrivate()),
    QWidget(parent)
{
    initWidget();
}

OnLineState::~OnLineState()
{

}

void OnLineState::setState(const OnlineStatus &state)
{
    MQ_D(OnLineState);
    d->setState(state);
    d->mainButton->setToolTip(d->stateTips[state]);
    d->mainButton->setIcon(QPixmap(d->stateFilePaths[state]));
}

OnlineStatus OnLineState::state()
{
    MQ_D(OnLineState);
    return d->state();
}

QMap<OnlineStatus,QString> OnLineState::onlineStatePixmap;

QString OnLineState::getStatePixmap(OnlineStatus state)
{
    return onlineStatePixmap.value(state);
}

void OnLineState::initWidget()
{
    MQ_D(OnLineState);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    d->mainButton = new QToolButton(this);
    d->mainButton->setToolTip(tr("Online State"));
    d->mainButton->setPopupMode(QToolButton::InstantPopup);
    d->mainButton->setArrowType(Qt::NoArrow);
    d->mainButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    d->mainButton->setStyleSheet("QToolButton::menu-indicator {image: none;}");
    d->mainButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);

    d->popMenu = new QMenu();

    if(!d->isInitPixmap)
    {
        onlineStatePixmap.insert(STATUS_ONLINE,QString(":/state/resource/icon/state/STATE_ONLINE.png"));
        onlineStatePixmap.insert(STATUS_AWAY,QString(":/state/resource/icon/state/STATE_AWAY.png"));
        onlineStatePixmap.insert(STATUS_OFFLINE,QString(":/state/resource/icon/state/STATE_OFFLINE.png"));
        onlineStatePixmap.insert(STATUS_BUSY,QString(":/state/resource/icon/state/STATE_BUSY.png"));
        onlineStatePixmap.insert(STATUS_HIDE,QString(":/state/resource/icon/state/STATE_HIDE.png"));
        onlineStatePixmap.insert(STATUS_NoDisturb,QString(":/state/resource/icon/state/STATE_NODISTURB.png"));
        d->isInitPixmap = true;
    }

    QAction * onlineAction = new QAction(tr("Online"));
    onlineAction->setData(STATUS_ONLINE);
    onlineAction->setIcon(QPixmap(onlineStatePixmap.value(STATUS_ONLINE)));
    d->stateFilePaths.insert(STATUS_ONLINE,onlineStatePixmap.value(STATUS_ONLINE));
    d->stateTips.insert(STATUS_ONLINE,onlineAction->text());
    connect(onlineAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * awayAction = new QAction(tr("Away"));
    awayAction->setData(STATUS_AWAY);
    awayAction->setIcon(QPixmap(onlineStatePixmap.value(STATUS_AWAY)));
    d->stateFilePaths.insert(STATUS_AWAY,onlineStatePixmap.value(STATUS_AWAY));
    d->stateTips.insert(STATUS_AWAY,awayAction->text());
    connect(awayAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * offlineAction = new QAction(tr("Offline"));
    offlineAction->setData(STATUS_OFFLINE);
    offlineAction->setIcon(QPixmap(onlineStatePixmap.value(STATUS_OFFLINE)));
    d->stateFilePaths.insert(STATUS_OFFLINE,onlineStatePixmap.value(STATUS_OFFLINE));
    d->stateTips.insert(STATUS_ONLINE,offlineAction->text());
    connect(offlineAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction *  busyAction = new QAction(tr("Busy"));
    busyAction->setData(STATUS_BUSY);
    busyAction->setIcon(QPixmap(onlineStatePixmap.value(STATUS_BUSY)));
    d->stateFilePaths.insert(STATUS_BUSY,onlineStatePixmap.value(STATUS_BUSY));
    d->stateTips.insert(STATUS_ONLINE,busyAction->text());
    connect(busyAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * hideAction = new QAction(tr("Hide"));
    hideAction->setData(STATUS_HIDE);
    hideAction->setIcon(QPixmap(onlineStatePixmap.value(STATUS_HIDE)));
    d->stateFilePaths.insert(STATUS_HIDE,onlineStatePixmap.value(STATUS_HIDE));
    d->stateTips.insert(STATUS_ONLINE,hideAction->text());
    connect(hideAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * noDisturbAction = new QAction(tr("NoDisturb"));
    noDisturbAction->setData(STATUS_NoDisturb);
    noDisturbAction->setIcon(QPixmap(onlineStatePixmap.value(STATUS_NoDisturb)));
    d->stateFilePaths.insert(STATUS_NoDisturb,onlineStatePixmap.value(STATUS_NoDisturb));
    d->stateTips.insert(STATUS_ONLINE,noDisturbAction->text());
    connect(noDisturbAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    d->popMenu->addAction(onlineAction);
    d->popMenu->addAction(awayAction);
    d->popMenu->addAction(busyAction);
    d->popMenu->addAction(hideAction);
    d->popMenu->addAction(noDisturbAction);
    d->popMenu->addAction(offlineAction);

    d->mainButton->setMenu(d->popMenu);
    layout->addWidget(d->mainButton);

    this->setLayout(layout);
}

/*!
 * @brief 切换显示用户状态
 * @return 无
 * @note 改变的状态与当前状态不一致时，才发出改变信号。
 */
void OnLineState::switchState(bool)
{
    MQ_D(OnLineState);
    QAction * action = dynamic_cast<QAction *>(QObject::sender());
    if(action)
    {
        OnlineStatus newState = (OnlineStatus)action->data().toInt();
        if(newState != d->state())
        {
            d->setState(newState);
            d->mainButton->setToolTip(action->text());
            d->mainButton->setIcon(action->icon());
            emit stateChanged(d->state());
        }
    }
}
