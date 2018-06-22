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

    class IpSettingContainer : public QWidget{
    public:
        explicit IpSettingContainer(QWidget * parent = nullptr):QWidget(parent){
            initWidget();
        }

        void setIpAndPort(QString ip,QString port){
            ipEdit->setText(ip);
            portEdit->setText(port);
        }

        bool isValidIp(){return RUtil::validateIpFormat(ipEdit->text());}
        bool isValidPort(){
            if(portEdit->text().size() <= 0)
                return false;
            return true;
        }

        QString getIp(){return ipEdit->text();}
        QString getPort(){return portEdit->text();}

    private:
        void initWidget(){

            ipTextLabel = new QLabel(this);
            ipTextLabel->setText(QObject::tr("Ip Address"));

            ipEdit = new QLineEdit(this);

            QString matchWholeIp = QString(Constant::FullIp_Reg).arg(Constant::SingleIp_Reg).arg(Constant::SingleIp_Reg);
            QRegExp rx(matchWholeIp);
            QRegExpValidator * ipValidator = new QRegExpValidator(rx);
            ipEdit->setValidator(ipValidator);

            portTextLabel = new QLabel(this);
            portTextLabel->setText(QObject::tr("Port"));

            portEdit = new QLineEdit(this);

            QIntValidator * portValidator = new QIntValidator(1024,65535);
            portEdit->setValidator(portValidator);

            QGridLayout * gridLayout = new QGridLayout;
            gridLayout->addWidget(ipTextLabel,0,0,1,1);
            gridLayout->addWidget(ipEdit,0,1,1,2);
            gridLayout->addWidget(portTextLabel,0,3,1,1);
            gridLayout->addWidget(portEdit,0,4,1,1);
            setLayout(gridLayout);
        }

    private:
        QLabel * ipTextLabel;
        QLineEdit * ipEdit;
        QLabel * portTextLabel;
        QLineEdit * portEdit;

    };

    QWidget * contentWidget;

    IpSettingContainer * textServerIp;

#ifndef __LOCAL_CONTACT__
    IpSettingContainer * fileServerIp;
#else
    IpSettingContainer * tandemServerIp1;
    IpSettingContainer * tandemServerIp2;
#endif
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

    /*******************文本服务器地址*************************/
    QLabel * textLabel = new QLabel(mainWidget);
    textLabel->setText(QObject::tr("Text Server"));

    textServerIp = new IpSettingContainer;

    /*******************文件服务器地址*************************/
    QLabel * fileLabel = new QLabel(mainWidget);
#ifdef __LOCAL_CONTACT__
    fileLabel->setText(QObject::tr("File Server"));

    tandemServerIp1 = new IpSettingContainer;
    tandemServerIp2 = new IpSettingContainer;
#else
    fileLabel->setText(QObject::tr("File Server"));
    fileServerIp = new IpSettingContainer;
#endif
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
    gridLayout->addWidget(textServerIp,1,1,1,5);

    gridLayout->addWidget(fileLabel,2,1,1,1);
#ifdef __LOCAL_CONTACT__
    gridLayout->addWidget(tandemServerIp1,3,1,1,5);
    gridLayout->addWidget(tandemServerIp2,4,1,1,5);

    gridLayout->setRowStretch(5,5);
    gridLayout->addWidget(bottomWidget,6,0,1,7);
#else
    gridLayout->addWidget(fileServerIp,3,1,1,5);
    gridLayout->setRowStretch(4,5);
    gridLayout->addWidget(bottomWidget,5,0,1,7);
#endif
    gridLayout->setColumnStretch(6,1);

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
    d->textServerIp->setIpAndPort(settings->value(Constant::SYSTEM_NETWORK_TEXT_IP,Constant::DEFAULT_NETWORK_TEXT_IP).toString(),
                                  settings->value(Constant::SYSTEM_NETWORK_TEXT_PORT,Constant::DEFAULT_NETWORK_TEXT_PORT).toString());
#ifdef __LOCAL_CONTACT__
    d->tandemServerIp1->setIpAndPort(settings->value(Constant::SYSTEM_NETWORK_TANDEM_IP1,"").toString(),
                                     settings->value(Constant::SYSTEM_NETWORK_TANDEM_PORT1,"").toString());

    d->tandemServerIp2->setIpAndPort(settings->value(Constant::SYSTEM_NETWORK_TANDEM_IP2,"").toString(),
                                     settings->value(Constant::SYSTEM_NETWORK_TANDEM_PORT2,"").toString());
#else
    d->fileServerIp->setIpAndPort(settings->value(Constant::SYSTEM_NETWORK_FILE_IP,Constant::DEFAULT_NETWORK_FILE_IP).toString(),
                                     settings->value(Constant::SYSTEM_NETWORK_FILE_PORT,Constant::DEFAULT_NETWORK_FILE_PORT).toString());
#endif
    RUtil::globalSettings()->endGroup();
}

void NetSettings::updateSettings()
{
    MQ_D(NetSettings);

    if(!d->textServerIp->isValidIp() || !d->textServerIp->isValidPort())
    {
        RMessageBox::warning(this,tr("Warning"),tr("ip address is error!"),RMessageBox::Yes);
        return;
    }

#ifdef __LOCAL_CONTACT__
    if((d->tandemServerIp1->getIp().size() > 0 || d->tandemServerIp1->getPort().size() > 0) && (!d->tandemServerIp1->isValidIp()
            || !d->tandemServerIp1->isValidPort()) ){
        RMessageBox::warning(this,tr("Warning"),tr("trandem ip1 address is error!"),RMessageBox::Yes);
        return;
    }

    if((d->tandemServerIp2->getIp().size() > 0 || d->tandemServerIp2->getPort().size() > 0) && (!d->tandemServerIp2->isValidIp()
            || !d->tandemServerIp2->isValidPort())){
        RMessageBox::warning(this,tr("Warning"),tr("trandem ip2 address is error!"),RMessageBox::Yes);
        return;
    }
#else
    if((d->fileServerIp->getIp().size() > 0 && !d->fileServerIp->isValidIp()) ||
            (d->fileServerIp->getPort().size() > 0 && !d->fileServerIp->isValidPort())){
        RMessageBox::warning(this,tr("Warning"),tr("file address is error!"),RMessageBox::Yes);
        return;
    }
#endif

    QSettings * settings =  RUtil::globalSettings();
    settings->beginGroup(Constant::SYSTEM_NETWORK);
    settings->setValue(Constant::SYSTEM_NETWORK_TEXT_IP,d->textServerIp->getIp());
    settings->setValue(Constant::SYSTEM_NETWORK_TEXT_PORT,d->textServerIp->getPort());

#ifndef __LOCAL_CONTACT__
    settings->setValue(Constant::SYSTEM_NETWORK_FILE_IP,d->fileServerIp->getIp());
    settings->setValue(Constant::SYSTEM_NETWORK_FILE_PORT,d->fileServerIp->getPort());
#else
    settings->setValue(Constant::SYSTEM_NETWORK_TANDEM_IP1,d->tandemServerIp1->getIp());
    settings->setValue(Constant::SYSTEM_NETWORK_TANDEM_PORT1,d->tandemServerIp1->getPort());

    settings->setValue(Constant::SYSTEM_NETWORK_TANDEM_IP2,d->tandemServerIp2->getIp());
    settings->setValue(Constant::SYSTEM_NETWORK_TANDEM_PORT2,d->tandemServerIp2->getPort());
#endif
    settings->endGroup();

    if(G_NetSettings.textServerIp != d->textServerIp->getIp() || G_NetSettings.textServerPort != d->textServerIp->getPort().toUShort())
    {
        G_NetSettings.textServerIp = d->textServerIp->getIp();
        G_NetSettings.textServerPort = d->textServerIp->getPort().toUShort();
#ifdef __LOCAL_CONTACT__
        G_NetSettings.tandemServerIp1 = d->tandemServerIp1->getIp();
        G_NetSettings.tandemServerPort1 = d->tandemServerIp1->getPort().toUShort();

        G_NetSettings.tandemServerIp2 = d->tandemServerIp2->getIp();
        G_NetSettings.tandemServerPort2 = d->tandemServerIp2->getPort().toUShort();
#else
        G_NetSettings.fileServerIp = d->fileServerIp->getIp();
        G_NetSettings.fileServerPort = d->fileServerIp->getPort().toUShort();
#endif
        TextNetConnector::instance()->disConnect();
    }

    emit ipInfoUpdated();
    RMessageBox::information(this,tr("Information"),tr("Save changes successfully!"),RMessageBox::Yes);

    close();
}
