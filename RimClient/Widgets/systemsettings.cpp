#include "systemsettings.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPainter>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "Util/rsingleton.h"
#include "Util/imagemanager.h"
#include "widget/rbutton.h"

SystemSettingsPage::SystemSettingsPage(QWidget *parent):QWidget(parent)
{
    itemCount = 0;
    mouseEnter = false;

    content = new QWidget(this);

    mainLayout = new QGridLayout;

    descLabel = new QLabel(content);
    descLabel->setFixedWidth(80);
    descLabel->setAlignment(Qt::AlignRight);

    mainLayout->addWidget(descLabel,0,0,1,1);

    mainLayout->setHorizontalSpacing(30);

    content->setLayout(mainLayout);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(content);
    setLayout(layout);
}

void SystemSettingsPage::setDescInfo(QString &text)
{
    descLabel->setText(text);
}

void SystemSettingsPage::addItem(QWidget *item)
{
    mainLayout->addWidget(item,itemCount++,1,1,2);
}

void SystemSettingsPage::paintEvent(QPaintEvent *event)
{
    if(mouseEnter)
    {
        QPainter painter(this);
        painter.drawRect(rect().adjusted(1,1,-1,-1));
    }
}

void SystemSettingsPage::enterEvent(QEvent *event)
{
    mouseEnter = true;
    update();
}

void SystemSettingsPage::leaveEvent(QEvent *event)
{
    mouseEnter = false;
    update();
}

class SystemSettingsPrivate : public GlobalData<SystemSettings>
{
    Q_DECLARE_PUBLIC(SystemSettings)

private:
    SystemSettingsPrivate(SystemSettings * q):q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    SystemSettings * q_ptr;
    QScrollArea * scrollArea;
};

void SystemSettingsPrivate::initWidget()
{
    scrollArea = new QScrollArea;

    QWidget * contentWidget = new QWidget;
    contentWidget->setObjectName("Widget_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(6,0,6,6);

    /****************登录设置********************/
    SystemSettingsPage * loginPage = new SystemSettingsPage(contentWidget);
    loginPage->setDescInfo(QObject::tr("Login"));

    QCheckBox * autoStartUp = new QCheckBox;
    autoStartUp->setText(QObject::tr("Boot automatically"));

    QCheckBox * autoLogin = new QCheckBox;
    autoLogin->setText(QObject::tr("Automatic login"));

    loginPage->addItem(autoStartUp);
    loginPage->addItem(autoLogin);

    /****************主页面设置********************/
    SystemSettingsPage * mainPanelPage = new SystemSettingsPage(contentWidget);
    mainPanelPage->setDescInfo(QObject::tr("Main Page"));

    QCheckBox * keepFront = new QCheckBox;
    keepFront->setText(QObject::tr("Stick to the top"));

    QCheckBox * exitSystem = new QCheckBox;
    exitSystem->setText(QObject::tr("Quit directly"));

    QCheckBox * systemTrayIcon = new QCheckBox;
    systemTrayIcon->setText(QObject::tr("Display taskbar icon"));

    mainPanelPage->addItem(keepFront);
    mainPanelPage->addItem(exitSystem);
    mainPanelPage->addItem(systemTrayIcon);

    /****************状态设置********************/
    SystemSettingsPage * statePage = new SystemSettingsPage(contentWidget);
    statePage->setDescInfo(QObject::tr("State"));

    RButton * autoReplyButt = new RButton();
    autoReplyButt->setText(QObject::tr("Automatic response"));

    statePage->addItem(autoReplyButt);

    /****************会话窗口设置********************/
    SystemSettingsPage * sessionPage = new SystemSettingsPage(contentWidget);
    sessionPage->setDescInfo(QObject::tr("Session"));

    QCheckBox * windowShaking = new QCheckBox;
    windowShaking->setText(QObject::tr("Allow window jitter"));

    sessionPage->addItem(windowShaking);

    /****************快捷键设置********************/
    SystemSettingsPage * shortCutPage = new SystemSettingsPage(contentWidget);
    shortCutPage->setDescInfo(QObject::tr("Shortcut"));

    RButton * shortcutButt = new RButton();
    shortcutButt->setText(QObject::tr("Shortcut Setting"));

    shortCutPage->addItem(shortcutButt);

    /****************声音设置********************/
    SystemSettingsPage * soundPage = new SystemSettingsPage(contentWidget);
    soundPage->setDescInfo(QObject::tr("Sound"));

    QCheckBox * soundAvailable = new QCheckBox;
    soundAvailable->setText(QObject::tr("Sound Available"));

    RButton * soundButt = new RButton();
    soundButt->setText(QObject::tr("Sound Setting"));

    soundPage->addItem(soundAvailable);
    soundPage->addItem(soundButt);

    /****************文件设置********************/
    SystemSettingsPage * filePage = new SystemSettingsPage(contentWidget);
    filePage->setDescInfo(QObject::tr("File"));

    RButton * fileButt = new RButton();
    fileButt->setText(QObject::tr("Change Recv Path"));

    filePage->addItem(fileButt);

    contentLayout->addWidget(loginPage);
    contentLayout->addWidget(mainPanelPage);
    contentLayout->addWidget(statePage);
    contentLayout->addWidget(sessionPage);
    contentLayout->addWidget(shortCutPage);
    contentLayout->addWidget(soundPage);
    contentLayout->addWidget(filePage);

    contentLayout->addStretch(1);
    contentWidget->setLayout(contentLayout);

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    q_ptr->setContentWidget(scrollArea);
}

#define SYSTEM_SETTING_WIDTH 380
#define SYSTEM_SETTING_HEIGHT 600

SystemSettings::SystemSettings(QWidget *parent):
    d_ptr(new SystemSettingsPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Settings"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    ToolBar * bar = enableToolBar(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowTitle(tr("Settings"));
    }

    setFixedSize(SYSTEM_SETTING_WIDTH,SYSTEM_SETTING_HEIGHT);
    QSize  screenSize = RUtil::screenSize();
    setGeometry((screenSize.width() - SYSTEM_SETTING_WIDTH)/2,(screenSize.height() - SYSTEM_SETTING_HEIGHT)/2,SYSTEM_SETTING_WIDTH,SYSTEM_SETTING_HEIGHT);
}

SystemSettings::~SystemSettings()
{

}

