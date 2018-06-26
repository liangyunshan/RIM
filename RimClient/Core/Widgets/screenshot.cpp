#include "screenshot.h"
#include "global.h"
#include "rsingleton.h"
#include "constants.h"
#include "actionmanager/shortcutsettings.h"

#include <QAction>
#include <QKeySequence>
#include <QClipboard>
#include <QMimeData>
#include <QProcess>
#include <QTimer>
#include <QKeyEvent>
#include <QApplication>

ScreenShot * p_ScreenShot=NULL;

ScreenShot::ScreenShot(QObject *parent) : QObject(parent)
{
    p_shotProcess = NULL;
    b_isScreeHide = false;
    p_shotTimer = NULL;

    p_ScreenShot = this;

    initKey();
    instanceHideWindowAction();
}

ScreenShot *ScreenShot::instance()
{
    if(p_ScreenShot == NULL)
    {
        p_ScreenShot = new ScreenShot();
    }
    G_pScreenShot = p_ScreenShot;
    return p_ScreenShot;
}

int ScreenShot::setScreenShotKey(QString key)
{
    return setScreenShotKey(QKeySequence(key));
}

int ScreenShot::setScreenShotKey(QKeySequence key)
{
    G_ScreenShotKeySequence = key.toString();
    instanceScreenShotAction()->setShortcut(key);

    RSingleton<ShortcutSettings>::instance()->removeShortCut(Constant::ACTION_CHAT_SCREEN_SHOT);
    RSingleton<ShortcutSettings>::instance()->addShortcut(Id(Constant::ACTION_CHAT_SCREEN_SHOT),key);
    RSingleton<ShortcutSettings>::instance()->save();
    return 0;
}

QAction *ScreenShot::instanceScreenShotAction()
{
    if(G_pScreenShotAction==NULL)
    {
        G_pScreenShotAction = new QAction(tr("screen shot"));
        if(G_pScreenShotAction->objectName().isEmpty())
        {
            G_pScreenShotAction->setObjectName(this->objectName() + "Screenshot");
        }
        connect(G_pScreenShotAction,SIGNAL(triggered(bool)),this,SLOT(slot_ScreenShot(bool)));
    }
    return G_pScreenShotAction;
}

QAction *ScreenShot::instanceHideWindowAction()
{
    if(G_pHideWindowAction==NULL)
    {
        G_pHideWindowAction = new QAction(tr("Hide current window while screenshot"));
        G_pHideWindowAction->setCheckable(true);
        if(G_pHideWindowAction->objectName().isEmpty())
        {
            G_pHideWindowAction->setObjectName(this->objectName() + "HideWindow");
        }
        connect(G_pHideWindowAction,SIGNAL(triggered(bool)),this,SLOT(slot_ScreenShotHide(bool)));
    }
    return G_pHideWindowAction;
}

void ScreenShot::initKey()
{
    QKeySequence key_Button_Chat_Shot
            = RSingleton<ShortcutSettings>::instance()->shortCut(Constant::ACTION_CHAT_SCREEN_SHOT);

    G_ScreenShotKeySequence = key_Button_Chat_Shot.toString();
    setScreenShotKey(G_ScreenShotKeySequence);
}

//截图
void ScreenShot::slot_ScreenShot(bool flag)
{
    Q_UNUSED(flag)

    if(p_shotProcess==NULL)
    {
        p_shotProcess = new QProcess();
        QObject::connect(p_shotProcess,SIGNAL(finished(int, QProcess::ExitStatus)),
                         this,SLOT(slot_ScreenShot_Ready(int, QProcess::ExitStatus)));
    }
    if(b_isScreeHide)
    {
        if(p_shotTimer == NULL)
        {
            p_shotTimer = new QTimer();
            QObject::connect(p_shotTimer,SIGNAL(timeout()),this,SLOT(slot_ScreenTimeout()));
        }
        //TODO LYS-20180604截图前隐藏当前全部显示的聊天窗口

        p_shotTimer->start(0.1*1000);
    }
    else
    {
        p_shotProcess->start( "rundll32 "+ QApplication::applicationDirPath()+ "/PrScrn.dll PrScrn" );
    }
}

void ScreenShot::slot_ScreenTimeout()
{
    p_shotTimer->stop();
    if(p_shotProcess)
    {
        p_shotProcess->start( "rundll32 "+ QApplication::applicationDirPath()+ "/PrScrn.dll PrScrn" );
    }
}

//截图完成
void ScreenShot::slot_ScreenShot_Ready(int , QProcess::ExitStatus)
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData->hasImage())
    {
        emit sig_ShotReady(true);
    }
    //TODO LYS-20180604截图完成后显示之前被隐藏的聊天窗口
}

//截屏时聊天界面是否隐藏
void ScreenShot::slot_ScreenShotHide(bool flag)
{
    b_isScreeHide = flag;
    G_pHideWindowAction->setChecked(flag);
}
