#include "systemsettings.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QPainter>
#include <QStackedWidget>
#include <QLineEdit>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QAction>

#include "head.h"
#include "global.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "widget/rbutton.h"
#include "panelcontentarea.h"
#include "subject.h"
#include "widget/rmessagebox.h"
#include "setkeysequencedialog.h"
#include "user/user.h"

SystemSettingsPage::SystemSettingsPage(QWidget *parent):QWidget(parent)
{
    itemCount = 0;
    rowCount = 0;
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

void SystemSettingsPage::setDescInfo(QString text)
{
    descLabel->setText(text);
}

void SystemSettingsPage::addItem(QWidget *item)
{
    mainLayout->addWidget(item,rowCount++,1,1,2);
}

void SystemSettingsPage::addItem(QWidget *item, int row, int column, int rowSpan, int columnSpan)
{
    mainLayout->addWidget(item,row,column,rowSpan,columnSpan);
    rowCount += rowSpan;
}

int SystemSettingsPage::row()
{
    return this->rowCount;
}

void SystemSettingsPage::paintEvent(QPaintEvent *)
{
    if(mouseEnter)
    {
        QPainter painter(this);
        painter.setPen(QPen(QColor(229,229,229),1));
        painter.drawRect(rect().adjusted(1,1,-2,-1));
    }
}

void SystemSettingsPage::enterEvent(QEvent *)
{
    mouseEnter = true;
    update();
}

void SystemSettingsPage::leaveEvent(QEvent *)
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
        localSettings();
    }

    void initWidget();
    void localSettings();

    PanelTabBar * tabBar;
    QStackedWidget * widgetContainer;

    SystemSettings * q_ptr;
    QScrollArea * basicScrollArea;
    QScrollArea * securityScrollArea;

    QCheckBox * autoStartUp;
    QCheckBox * autoLogin;

    QCheckBox * keepFront;
    QCheckBox * exitSystem;
    QCheckBox * systemTrayIcon;
    QCheckBox * hidePanel;

    QCheckBox * windowShaking;

    QCheckBox * soundAvailable;

    QCheckBox * lockCheckBox;

    QCheckBox * recordCheckBox;
    SetKeySequenceDialog *p_setKeySequenceDialog;
};

void SystemSettingsPrivate::initWidget()
{
    QWidget * contentWidget = new QWidget;
    contentWidget->setObjectName("Widget_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    contentWidget->setLayout(contentLayout);

    tabBar = new PanelTabBar;

    PanelTabItem * baseInfoItem = tabBar->addTab();
    baseInfoItem->setTextVisible(true);
    baseInfoItem->setObjectName("settings_setting");
    baseInfoItem->setText(QObject::tr("Basic Settings"));
    baseInfoItem->setToolTip(QObject::tr("Basic Settings"));

    PanelTabItem * securityItem = tabBar->addTab();
    securityItem->setObjectName("settings_security");
    securityItem->setTextVisible(true);
    securityItem->setText(QObject::tr("Security Settings"));
    securityItem->setToolTip(QObject::tr("Security Settings"));

    QWidget * basicWidget = new QWidget();
    basicWidget->setObjectName("Widget_ContentWidget");

    QWidget * securityWidget = new QWidget();
    securityWidget->setObjectName("Widget_ContentWidget");

    /****************基础信息页面********************/
    basicScrollArea = new QScrollArea;

    QVBoxLayout * basicLayout = new QVBoxLayout;
    basicLayout->setContentsMargins(0,0,0,6);

    /****************登录设置********************/
    SystemSettingsPage * loginPage = new SystemSettingsPage(basicWidget);
    loginPage->setDescInfo(QObject::tr("Login"));

    autoStartUp = new QCheckBox;
    autoStartUp->setText(QObject::tr("Boot automatically"));
    QObject::connect(autoStartUp,SIGNAL(clicked(bool)),q_ptr,SLOT(respAutoStartUp(bool)));

    autoLogin = new QCheckBox;
    autoLogin->setText(QObject::tr("Automatic login"));
    QObject::connect(autoLogin,SIGNAL(clicked(bool)),q_ptr,SLOT(respAutoLogIn(bool)));

    loginPage->addItem(autoStartUp);
    loginPage->addItem(autoLogin);

    /****************主页面设置********************/
    SystemSettingsPage * mainPanelPage = new SystemSettingsPage(basicWidget);
    mainPanelPage->setDescInfo(QObject::tr("Main Page"));

    keepFront = new QCheckBox;
    keepFront->setText(QObject::tr("Stick to the top"));
    QObject::connect(keepFront,SIGNAL(clicked(bool)),q_ptr,SLOT(respKeepFront(bool)));

    exitSystem = new QCheckBox;
    exitSystem->setText(QObject::tr("Quit directly"));
    QObject::connect(exitSystem,SIGNAL(clicked(bool)),q_ptr,SLOT(respExitSystem(bool)));

    systemTrayIcon = new QCheckBox;
    systemTrayIcon->setText(QObject::tr("Display taskbar icon"));
    QObject::connect(systemTrayIcon,SIGNAL(clicked(bool)),q_ptr,SLOT(respSystemTrayIcon(bool)));

    hidePanel = new QCheckBox;
    hidePanel->setText(QObject::tr("Automatically hides panel attached to edge"));
    QObject::connect(hidePanel,SIGNAL(clicked(bool)),q_ptr,SLOT(respHidePanel(bool)));

    mainPanelPage->addItem(keepFront);
    mainPanelPage->addItem(exitSystem);
    mainPanelPage->addItem(systemTrayIcon);
    mainPanelPage->addItem(hidePanel);

    /****************状态设置********************/
    SystemSettingsPage * statePage = new SystemSettingsPage(basicWidget);
    statePage->setDescInfo(QObject::tr("State"));

    RButton * autoReplyButt = new RButton();
    autoReplyButt->setText(QObject::tr("Automatic response"));
    QObject::connect(autoReplyButt,SIGNAL(pressed()),q_ptr,SLOT(respAutoReply()));

    statePage->addItem(autoReplyButt);

    /****************会话窗口设置********************/
    SystemSettingsPage * sessionPage = new SystemSettingsPage(basicWidget);
    sessionPage->setDescInfo(QObject::tr("Session"));

    windowShaking = new QCheckBox;
    windowShaking->setText(QObject::tr("Allow window jitter"));
    QObject::connect(windowShaking,SIGNAL(clicked(bool)),q_ptr,SLOT(respWindowShake(bool)));

    sessionPage->addItem(windowShaking);

    /****************快捷键设置********************/
    SystemSettingsPage * shortCutPage = new SystemSettingsPage(basicWidget);
    shortCutPage->setDescInfo(QObject::tr("Shortcut"));

    RButton * shortcutButt = new RButton();
    shortcutButt->setText(QObject::tr("Shortcut Setting"));
    QObject::connect(shortcutButt,SIGNAL(pressed()),q_ptr,SLOT(respShortCut()));

    shortCutPage->addItem(shortcutButt);

    //TODO:需要将界面的背景替换为统一的白色
    p_setKeySequenceDialog = new SetKeySequenceDialog();

    /****************声音设置********************/
    SystemSettingsPage * soundPage = new SystemSettingsPage(basicWidget);
    soundPage->setDescInfo(QObject::tr("Sound"));

    soundAvailable = new QCheckBox;
    soundAvailable->setText(QObject::tr("Sound Available"));
    QObject::connect(soundAvailable,SIGNAL(clicked(bool)),q_ptr,SLOT(respSoundAvailable(bool)));

    RButton * soundButt = new RButton();
    soundButt->setText(QObject::tr("Sound Setting"));
    QObject::connect(soundButt,SIGNAL(pressed()),q_ptr,SLOT(respSoundSetting()));

    soundPage->addItem(soundAvailable);
    soundPage->addItem(soundButt);

    /****************文件设置********************/
    SystemSettingsPage * filePage = new SystemSettingsPage(basicWidget);
    filePage->setDescInfo(QObject::tr("File"));

    RButton * fileButt = new RButton();
    fileButt->setText(QObject::tr("Change Recv Path"));
    QObject::connect(fileButt,SIGNAL(pressed()),q_ptr,SLOT(respFileSetting()));

    filePage->addItem(fileButt);

    basicLayout->addWidget(loginPage);
    basicLayout->addWidget(mainPanelPage);
    basicLayout->addWidget(statePage);
    basicLayout->addWidget(sessionPage);
    basicLayout->addWidget(shortCutPage);
    basicLayout->addWidget(soundPage);
    basicLayout->addWidget(filePage);

    basicLayout->addStretch(1);
    basicWidget->setLayout(basicLayout);

    basicScrollArea->setWidget(basicWidget);
    basicScrollArea->setWidgetResizable(true);

    /****************安全设置页面********************/
    securityScrollArea = new QScrollArea;

    QVBoxLayout * securityLayout = new QVBoxLayout;
    securityLayout->setContentsMargins(0,0,0,6);

    /****************密码设置********************/
    SystemSettingsPage * passwordPage = new SystemSettingsPage(securityWidget);
    passwordPage->setDescInfo(QObject::tr("Password"));

    QLabel * originPass = new QLabel(passwordPage);
    originPass->setText(QObject::tr("Original password"));

    QLineEdit * originPassEdit = new QLineEdit(passwordPage);
    originPassEdit->setEchoMode(QLineEdit::Password);

    passwordPage->addItem(originPass);
    passwordPage->addItem(originPassEdit,passwordPage->row(),1,1,2);

    QLabel * newPass = new QLabel(passwordPage);
    newPass->setText(QObject::tr("New password"));

    QLineEdit * newPassEdit = new QLineEdit(passwordPage);
    newPassEdit->setEchoMode(QLineEdit::Password);

    passwordPage->addItem(newPass);
    passwordPage->addItem(newPassEdit,passwordPage->row(),1,1,2);

    QLabel * confirmPass = new QLabel(passwordPage);
    confirmPass->setText(QObject::tr("Confirm password"));

    QLineEdit * confirmPassEdit = new QLineEdit(passwordPage);
    confirmPassEdit->setEchoMode(QLineEdit::Password);

    passwordPage->addItem(confirmPass);
    passwordPage->addItem(confirmPassEdit,passwordPage->row(),1,1,2);

    RButton * confirmButton = new RButton();
    confirmButton->setText(QObject::tr("Confirm the changes"));
    passwordPage->addItem(confirmButton);

    /****************程序锁********************/
    SystemSettingsPage * lockPage = new SystemSettingsPage(securityWidget);
    QString lockInfo = QString(Constant::AppShortName) + QObject::tr("Lock");
    lockPage->setDescInfo(lockInfo);

    lockCheckBox = new QCheckBox;
    lockCheckBox->setText(QObject::tr("Turn on the device lock"));
    QObject::connect(lockCheckBox,SIGNAL(clicked(bool)),q_ptr,SLOT(respSystemLock(bool)));

    lockPage->addItem(lockCheckBox);

    /****************消息记录********************/
    SystemSettingsPage * recordPage = new SystemSettingsPage(securityWidget);
    recordPage->setDescInfo(QObject::tr("Message record"));

    recordCheckBox = new QCheckBox;
    recordCheckBox->setText(QObject::tr("Exit to delete the record"));
    QObject::connect(recordCheckBox,SIGNAL(clicked(bool)),q_ptr,SLOT(respDelRecord(bool)));

    recordPage->addItem(recordCheckBox);

    securityLayout->addWidget(passwordPage);
    securityLayout->addWidget(lockPage);
    securityLayout->addWidget(recordPage);
    securityLayout->addStretch(1);

    securityWidget->setLayout(securityLayout);

    securityScrollArea->setWidget(securityWidget);
    securityScrollArea->setWidgetResizable(true);

    widgetContainer = new QStackedWidget;
    widgetContainer->addWidget(basicScrollArea);
    widgetContainer->addWidget(securityScrollArea);

    tabBar->setCurrentIndex(0);
    QObject::connect(tabBar,SIGNAL(currentIndexChanged(int)),widgetContainer,SLOT(setCurrentIndex(int)));

    contentLayout->addWidget(tabBar);
    contentLayout->addWidget(widgetContainer);

    q_ptr->setContentWidget(contentWidget);
}

void SystemSettingsPrivate::localSettings()
{
    QSettings * settings = G_User->getSettings();
    settings->beginGroup(Constant::USER_SETTING_GROUP);

    autoStartUp->setChecked(settings->value(Constant::USER_SETTING_AUTO_STARTUP,false).toBool());
    autoLogin->setChecked(settings->value(Constant::USER_SETTING_AUTO_LOGIN,false).toBool());

    keepFront->setChecked(settings->value(Constant::USER_SETTING_TOPHINT,false).toBool());
    exitSystem->setChecked(settings->value(Constant::USER_SETTING_EXIT_SYSTEM,false).toBool());
    hidePanel->setChecked(settings->value(Constant::USER_SETTING_HIDEPANEL,false).toBool());

    windowShaking->setChecked(settings->value(Constant::USER_SETTING_WINDOW_SHAKE,false).toBool());

    soundAvailable->setChecked(settings->value(Constant::USER_SETTING_SOUND_AVAILABLE,false).toBool());

    lockCheckBox->setChecked(settings->value(Constant::USER_SETTING_SYSTEM_LOCK,false).toBool());

    recordCheckBox->setChecked(settings->value(Constant::USER_SETTING_EXIT_DELRECORD,false).toBool());

    settings->endGroup();

    systemTrayIcon->setChecked(RUtil::globalSettings()->value(Constant::SETTING_TRAYICON,false).toBool());
}

#define SYSTEM_USER_BASIC_WIDTH 380
#define SYSTEM_USER_BASIC_HEIGHT 600

SystemSettings::SystemSettings(QWidget *parent):
    d_ptr(new SystemSettingsPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
//    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Settings"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowTitle(tr("Settings"));
    }

    RSingleton<Subject>::instance()->attach(this);

    setFixedSize(SYSTEM_USER_BASIC_WIDTH,SYSTEM_USER_BASIC_HEIGHT);
    QSize  screenSize = RUtil::screenSize();
    setGeometry((screenSize.width() - SYSTEM_USER_BASIC_WIDTH)/2,(screenSize.height() - SYSTEM_USER_BASIC_HEIGHT)/2,SYSTEM_USER_BASIC_WIDTH,SYSTEM_USER_BASIC_HEIGHT);
}

SystemSettings::~SystemSettings()
{
    RSingleton<Subject>::instance()->detach(this);
}

void SystemSettings::onMessage(MessageType)
{

}

void SystemSettings::respAutoStartUp(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_AUTO_STARTUP,flag);

#ifdef Q_OS_WIN32
    QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if(flag)
    {
       qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<qApp->applicationName()<<"__"<<QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
       settings.setValue(qApp->applicationName(), QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    }
    else
    {
        settings.remove(qApp->applicationName());
    }
    settings.sync();
#endif
}

void SystemSettings::respAutoLogIn(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_AUTO_LOGIN,flag);
}

void SystemSettings::respKeepFront(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_TOPHINT,flag);
    RSingleton<Subject>::instance()->notify(MessageType::MESS_SETTINGS);
}

void SystemSettings::respExitSystem(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_EXIT_SYSTEM,flag);
}

void SystemSettings::respSystemTrayIcon(bool flag)
{
    RUtil::globalSettings()->setValue(Constant::SETTING_TRAYICON,flag);
    RSingleton<Subject>::instance()->notify(MessageType::MESS_SETTINGS);
}

void SystemSettings::respHidePanel(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_HIDEPANEL,flag);
    RSingleton<Subject>::instance()->notify(MessageType::MESS_SETTINGS);
}

void SystemSettings::respWindowShake(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_WINDOW_SHAKE,flag);
}

void SystemSettings::respSoundAvailable(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_SOUND_AVAILABLE,flag);
}

void SystemSettings::respSystemLock(bool flag)
{
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_SYSTEM_LOCK,flag);
    if(flag)
    {
        RMessageBox::information(this,tr("Information"),tr("Use account password to unlock!"),RMessageBox::Yes);
    }
}

void SystemSettings::respDelRecord(bool flag)
{
    MQ_D(SystemSettings);

    if(flag)
    {
       int result = RMessageBox::warning(this,tr("Warning"),tr("After setting the chat history will not be retained, whether to continue setting?"),RMessageBox::Yes|RMessageBox::No,RMessageBox::No);

       if(result != RMessageBox::Yes)
       {
           d->recordCheckBox->setChecked(false);
       }
       else
       {
           d->recordCheckBox->setChecked(true);
       }
    }
    G_User->setSettingValue(Constant::USER_SETTING_GROUP,Constant::USER_SETTING_EXIT_DELRECORD,flag);
}

void SystemSettings::respAutoReply()
{

}

void SystemSettings::respShortCut()
{
    if(d_ptr->p_setKeySequenceDialog)
    {
        d_ptr->p_setKeySequenceDialog->showNormal();
    }

}

void SystemSettings::respSoundSetting()
{

}

void SystemSettings::respFileSetting()
{

}

