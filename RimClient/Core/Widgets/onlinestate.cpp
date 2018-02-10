#include "onlinestate.h"

#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QMetaEnum>
#include <QHBoxLayout>
#include <QMap>

#include "datastruct.h"
#include "head.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "constants.h"

class OnLineStatePrivate : public QObject,public GlobalData<OnLineState>
{
    Q_DECLARE_PUBLIC(OnLineState)
public:
    OnLineStatePrivate(){}
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
    mainButton->setIcon(QPixmap(d->stateFilePaths[state]));
}

OnlineStatus OnLineState::state()
{
    MQ_D(OnLineState);
    return d->state();
}

void OnLineState::initWidget()
{
    MQ_D(OnLineState);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    mainButton = new QToolButton(this);
    mainButton->setToolTip(tr("Online State"));
    mainButton->setPopupMode(QToolButton::InstantPopup);
    mainButton->setArrowType(Qt::NoArrow);
    mainButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    mainButton->setStyleSheet("QToolButton::menu-indicator {image: none;}");
    mainButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);

    popMenu = new QMenu();

    QAction * onlineAction = new QAction(tr("Online"));
    onlineAction->setData(STATUS_ONLINE);
    QString onlinePath = QString(":/state/resource/icon/state/STATE_ONLINE.png");
    onlineAction->setIcon(QPixmap(onlinePath));
    d->stateFilePaths.insert(STATUS_ONLINE,onlinePath);
    connect(onlineAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * offlineAction = new QAction(tr("Offline"));
    offlineAction->setData(STATUS_OFFLINE);
    QString offlinePath = QString(":/state/resource/icon/state/STATE_OFFLINE.png");
    offlineAction->setIcon(QPixmap(offlinePath));
    d->stateFilePaths.insert(STATUS_OFFLINE,offlinePath);
    connect(offlineAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction *  busyAction = new QAction(tr("Busy"));
    busyAction->setData(STATUS_BUSY);
    QString busyPath = QString(":/state/resource/icon/state/STATE_BUSY.png");
    busyAction->setIcon(QPixmap(busyPath));
    d->stateFilePaths.insert(STATUS_BUSY,busyPath);
    connect(busyAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * hideAction = new QAction(tr("Hide"));
    hideAction->setData(STATUS_INVISIBLE);
    QString hidePath = QString(":/state/resource/icon/state/STATE_HIDE.png");
    hideAction->setIcon(QPixmap(hidePath));
    d->stateFilePaths.insert(STATUS_INVISIBLE,hidePath);
    connect(hideAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * noDisturbAction = new QAction(tr("Away"));
    noDisturbAction->setData(STATUS_AWAY);
    QString noDisturbPath = QString(":/state/resource/icon/state/STATE_NODISTURB.png");
    noDisturbAction->setIcon(QPixmap(noDisturbPath));
    d->stateFilePaths.insert(STATUS_AWAY,noDisturbPath);
    connect(noDisturbAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    popMenu->addAction(onlineAction);
    popMenu->addAction(offlineAction);
    popMenu->addAction(busyAction);
    popMenu->addAction(hideAction);
    popMenu->addAction(noDisturbAction);

    mainButton->setMenu(popMenu);
    layout->addWidget(mainButton);

    this->setLayout(layout);
}

void OnLineState::switchState(bool)
{
    MQ_D(OnLineState);
    QAction * action = dynamic_cast<QAction *>(QObject::sender());
    if(action)
    {
        d->setState((OnlineStatus)action->data().toInt());
        mainButton->setIcon(action->icon());
        emit stateChanged(d->state());
    }
}
