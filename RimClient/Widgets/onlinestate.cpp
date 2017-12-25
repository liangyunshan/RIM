#include "onlinestate.h"

#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QMetaEnum>
#include <QDebug>
#include <QHBoxLayout>
#include <QMap>

#include "datastruct.h"
#include "head.h"
#include "Util/rsingleton.h"
#include "Util/imagemanager.h"
#include "constants.h"

class OnLineStatePrivate : public QObject,public GlobalData<OnLineState>
{
    Q_DECLARE_PUBLIC(OnLineState)
public:
    OnLineStatePrivate(){}
    ~OnLineStatePrivate(){}

    void setState(const OnLineState::UserState state)
    {
        this->currState = state;
    }

    OnLineState::UserState state()
    {
        return this->currState;
    }

private:
    OnLineState::UserState currState;

    QStringList stateName;

    QMap<OnLineState::UserState,QString> stateFilePaths;
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

void OnLineState::setState(const OnLineState::UserState &state)
{
    MQ_D(OnLineState);
    d->setState(state);
    mainButton->setIcon(QPixmap(d->stateFilePaths[state]));
}

OnLineState::UserState OnLineState::state()
{
    MQ_D(OnLineState);
    return d->state();
}

void OnLineState::initWidget()
{
    MQ_D(OnLineState);

    d->stateName<<QObject::tr("Online")<<QObject::tr("Offline")<<QObject::tr("Busy")
               <<QObject::tr("Hide")<<QObject::tr("NoDisturb");

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

#if QT_VERSION >= 0x050500
    QMetaEnum metaEnum = QMetaEnum::fromType<OnLineState::UserState>();

    for(int i = 0; i < metaEnum.keyCount(); i++)
    {
        QAction * action = new QAction(d->stateName.at(i));

        QString path = QString(":/state/resource/icon/state/")+metaEnum.key(i)+QString(".png");

        action->setIcon(QPixmap(path));
        action->setData(metaEnum.value(i));
        action->setToolTip(d->stateName.at(i));
        connect(action,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

        d->stateFilePaths.insert((UserState)metaEnum.value(i),path);

        if(i == 0)
        {
            mainButton->setIcon(QPixmap(QString(":/state/resource/icon/state/")+metaEnum.key(i)+QString(".png")));
        }

        popMenu->addAction(action);
    }
#else
    QAction * onlineAction = new QAction(tr("Online"));
    onlineAction->setData(UserState::STATE_ONLINE);
    onlineAction->setIcon(QPixmap(QString(":/state/resource/icon/state/STATE_ONLINE.png")));
    connect(onlineAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * offlineAction = new QAction(tr("Offline"));
    offlineAction->setData(UserState::STATE_OFFLINE);
    offlineAction->setIcon(QPixmap(QString(":/state/resource/icon/state/STATE_OFFLINE.png")));
    connect(offlineAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction *  busyAction = new QAction(tr("Busy"));
    busyAction->setData(UserState::STATE_BUSY);
    busyAction->setIcon(QPixmap(QString(":/state/resource/icon/state/STATE_BUSY.png")));
    connect(busyAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * hideAction = new QAction(tr("Hide"));
    hideAction->setData(UserState::STATE_HIDE);
    hideAction->setIcon(QPixmap(QString(":/state/resource/icon/state/STATE_HIDE.png")));
    connect(hideAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    QAction * noDisturbAction = new QAction(tr("NoDisturb"));
    noDisturbAction->setData(UserState::STATE_NODISTURB);
    noDisturbAction->setIcon(QPixmap(QString(":/state/resource/icon/state/STATE_NODISTURB.png")));
    connect(noDisturbAction,SIGNAL(triggered(bool)),this,SLOT(switchState(bool)));

    popMenu->addAction(onlineAction);
    popMenu->addAction(offlineAction);
    popMenu->addAction(busyAction);
    popMenu->addAction(hideAction);
    popMenu->addAction(noDisturbAction);
#endif

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
        d->setState((UserState)action->data().toInt());
        mainButton->setIcon(action->icon());
        emit stateChanged(d->state());
    }
}
