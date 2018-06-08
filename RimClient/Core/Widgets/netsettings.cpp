#include "netsettings.h"

#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QSettings>
#include <QIntValidator>
#include <QRegExpValidator>
#include <QDebug>

#include "constants.h"
#include "head.h"
#include "datastruct.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "widget/rbutton.h"
#include "rsingleton.h"
#include "Util/rutil.h"
#include "global.h"
#include "Network/netconnector.h"

#include "Util/rlog.h"
#include "widget/rmessagebox.h"
#include "messdiapatch.h"
#include "Network/netconnector.h"

class NetSettingsPrivate : public GlobalData<NetSettings>
{
    Q_DECLARE_PUBLIC(NetSettings)
public:
    NetSettingsPrivate(NetSettings * q):q_ptr(q)
    {
        initWidget();
    }

    NetSettings * q_ptr;

private:
    void initWidget();

    QWidget * contentWidget;
    QLineEdit * tcpTextServerIpEdit;
    QLineEdit * tcpTextServerPortEdit;

    QLineEdit * tcpFileServerIpEdit;
    QLineEdit * tcpFileServerPortEdit;

    RButton * saveButton;
    RButton * closeButton;
};

void NetSettingsPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Widget_ContentWidget");
    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    q_ptr->setContentWidget(contentWidget);

    QGridLayout * gridLayout = new QGridLayout();

    QWidget * mainWidget = new QWidget(contentWidget);

    QLabel * textLabel = new QLabel(mainWidget);
    textLabel->setText(QObject::tr("Text Server"));

    QLabel * ipTextLabel = new QLabel(mainWidget);
    ipTextLabel->setText(QObject::tr("Ip Address"));

    QLabel * portTextLabel = new QLabel(mainWidget);
    portTextLabel->setText(QObject::tr("Port"));

    /*******************文本服务器地址*************************/
    tcpTextServerIpEdit = new QLineEdit(mainWidget);

    QString matchWholeIp = QString(Constant::FullIp_Reg).arg(Constant::SingleIp_Reg).arg(Constant::SingleIp_Reg);
    QRegExp rx(matchWholeIp);
    QRegExpValidator * ipValidator = new QRegExpValidator(rx);
    tcpTextServerIpEdit->setValidator(ipValidator);

    tcpTextServerPortEdit = new QLineEdit(mainWidget);

    QIntValidator * portValidator = new QIntValidator(1024,65535);
    tcpTextServerPortEdit->setValidator(portValidator);

    /*******************文件服务器地址*************************/
    QLabel * fileLabel = new QLabel(mainWidget);
    fileLabel->setText(QObject::tr("File Server"));

    QLabel * ipFileLabel = new QLabel(mainWidget);
    ipFileLabel->setText(QObject::tr("Ip Address"));

    QLabel * portFileLabel = new QLabel(mainWidget);
    portFileLabel->setText(QObject::tr("Port"));

    tcpFileServerIpEdit = new QLineEdit(mainWidget);
    tcpFileServerIpEdit->setValidator(ipValidator);

    tcpFileServerPortEdit = new QLineEdit(mainWidget);
    tcpFileServerPortEdit->setValidator(portValidator);

    /***************按钮区******************/
    QWidget * bottomWidget = new QWidget(contentWidget);

    saveButton = new RButton();
    saveButton->setText(QObject::tr("Save"));

    QObject::connect(saveButton,SIGNAL(pressed()),q_ptr,SLOT(updateSettings()));

    closeButton = new RButton();
    closeButton->setText(QObject::tr("Cancel"));
    QObject::connect(closeButton,SIGNAL(pressed()),q_ptr,SLOT(close()));

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addStretch(1);
    layout->addWidget(saveButton);
    layout->addWidget(closeButton);

    bottomWidget->setLayout(layout);

    gridLayout->setColumnStretch(0,1);

    gridLayout->addWidget(textLabel,0,1,1,1);

    gridLayout->addWidget(ipTextLabel,1,1,1,1);
    gridLayout->addWidget(tcpTextServerIpEdit,1,2,1,2);
    gridLayout->addWidget(portTextLabel,1,4,1,1);
    gridLayout->addWidget(tcpTextServerPortEdit,1,5,1,1);

    gridLayout->addWidget(fileLabel,2,1,1,1);

    gridLayout->addWidget(ipFileLabel,3,1,1,1);
    gridLayout->addWidget(tcpFileServerIpEdit,3,2,1,2);
    gridLayout->addWidget(portFileLabel,3,4,1,1);
    gridLayout->addWidget(tcpFileServerPortEdit,3,5,1,1);

    gridLayout->setColumnStretch(6,1);

    gridLayout->setRowStretch(4,5);

    gridLayout->addWidget(bottomWidget,5,0,1,7);

    mainWidget->setLayout(gridLayout);

    contentLayout->addWidget(mainWidget);
    contentWidget->setLayout(contentLayout);
}

NetSettings::NetSettings(QWidget * parent):Widget(parent),
    d_ptr(new NetSettingsPrivate(this))
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Network settings"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));
    setWindowsMoveable(false);

    setFixedSize(Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_MESSAGEBOX);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));

        bar->setWindowTitle(windowTitle());
    }
}

NetSettings::~NetSettings()
{

}

void NetSettings::initLocalSettings()
{
    MQ_D(NetSettings);
    QSettings * settings =  RUtil::globalSettings();

    RUtil::globalSettings()->beginGroup(Constant::SYSTEM_NETWORK);
    d->tcpTextServerIpEdit->setText(settings->value(Constant::SYSTEM_NETWORK_TEXT_IP,Constant::DEFAULT_NETWORK_TEXT_IP).toString());
    d->tcpTextServerPortEdit->setText(settings->value(Constant::SYSTEM_NETWORK_TEXT_PORT,Constant::DEFAULT_NETWORK_TEXT_PORT).toString());

    d->tcpFileServerIpEdit->setText(settings->value(Constant::SYSTEM_NETWORK_FILE_IP,Constant::DEFAULT_NETWORK_FILE_IP).toString());
    d->tcpFileServerPortEdit->setText(settings->value(Constant::SYSTEM_NETWORK_FILE_PORT,Constant::DEFAULT_NETWORK_FILE_PORT).toString());
    RUtil::globalSettings()->endGroup();
}

void NetSettings::updateSettings()
{
    MQ_D(NetSettings);

    if(!RUtil::validateIpFormat(d->tcpTextServerIpEdit->text()))
    {
        RMessageBox::warning(this,tr("Warning"),tr("ip address is error!"),RMessageBox::Yes);
        return;
    }

    QSettings * settings =  RUtil::globalSettings();
    settings->beginGroup(Constant::SYSTEM_NETWORK);
    settings->setValue(Constant::SYSTEM_NETWORK_TEXT_IP,d->tcpTextServerIpEdit->text());
    settings->setValue(Constant::SYSTEM_NETWORK_TEXT_PORT,d->tcpTextServerPortEdit->text());

    settings->setValue(Constant::SYSTEM_NETWORK_FILE_IP,d->tcpFileServerIpEdit->text());
    settings->setValue(Constant::SYSTEM_NETWORK_FILE_PORT,d->tcpFileServerPortEdit->text());

    settings->endGroup();

    if(G_TextServerIp != d->tcpTextServerIpEdit->text() || G_TextServerPort != d->tcpTextServerPortEdit->text().toUShort())
    {
        G_TextServerIp = d->tcpTextServerIpEdit->text();
        G_TextServerPort = d->tcpTextServerPortEdit->text().toUShort();

        TextNetConnector::instance()->disConnect();
    }

    RMessageBox::information(this,tr("Information"),tr("Save changes successfully!"),RMessageBox::Yes);

    close();
}
