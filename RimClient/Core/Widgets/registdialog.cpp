#include "registdialog.h"

#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>

#include "constants.h"
#include "head.h"
#include "datastruct.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "widget/rbutton.h"
#include "rsingleton.h"
#include "Util/rutil.h"
#include "Network/netconnector.h"

#include "protocoldata.h"
#include "Network/msgwrap.h"
#include "Util/rlog.h"
#include "widget/rmessagebox.h"
#include "messdiapatch.h"

using namespace ProtocolType;

class RegistDialogPrivate : public GlobalData<RegistDialog>
{
    Q_DECLARE_PUBLIC(RegistDialog)
public:
    RegistDialogPrivate(RegistDialog * q):q_ptr(q)
    {
        initWidget();
    }

    RegistDialog * q_ptr;

private:
    void initWidget();

    QWidget * contentWidget;

    QLineEdit * nickNameEdit;
    QLineEdit * passwordEdit;
    QLineEdit * confirmPassEdit;

    RButton * confirmButt;
};

void RegistDialogPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Widget_ContentWidget");
    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    q_ptr->setContentWidget(contentWidget);

    QGridLayout * gridLayout = new QGridLayout();

    QWidget * mainWidget = new QWidget(contentWidget);

    QLabel * nickLabel = new QLabel(mainWidget);
    nickLabel->setFixedHeight(Constant::ITEM_FIX_HEIGHT);
    nickLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    nickLabel->setText(QObject::tr("Nick name"));

    nickNameEdit = new QLineEdit(mainWidget);
    nickNameEdit->setFixedHeight(Constant::ITEM_FIX_HEIGHT);
    nickNameEdit->setPlaceholderText(QObject::tr("Input nickname"));

    QLabel * passwordLabel = new QLabel(mainWidget);
    passwordLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    passwordLabel->setFixedHeight(Constant::ITEM_FIX_HEIGHT);
    passwordLabel->setText(QObject::tr("Password"));

    passwordEdit = new QLineEdit(mainWidget);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setFixedHeight(Constant::ITEM_FIX_HEIGHT);
    passwordEdit->setPlaceholderText(QObject::tr("Input password"));

    QLabel * confirmPasswordLabel = new QLabel(mainWidget);
    confirmPasswordLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    confirmPasswordLabel->setFixedHeight(Constant::ITEM_FIX_HEIGHT);
    confirmPasswordLabel->setText(QObject::tr("Confirm password"));

    confirmPassEdit = new QLineEdit(mainWidget);
    confirmPassEdit->setEchoMode(QLineEdit::Password);
    confirmPassEdit->setFixedHeight(Constant::ITEM_FIX_HEIGHT);
    confirmPassEdit->setPlaceholderText(QObject::tr("Input password"));

    confirmButt = new RButton(mainWidget);
    confirmButt->setText(QObject::tr("Sin up"));
    confirmButt->setMaximumWidth(1000);

    QObject::connect(confirmButt,SIGNAL(pressed()),q_ptr,SLOT(respValidInfo()));

    gridLayout->setRowStretch(0,1);
    gridLayout->setColumnStretch(0,1);

    gridLayout->setVerticalSpacing(20);

    gridLayout->addWidget(nickLabel,1,1,1,1);
    gridLayout->addWidget(nickNameEdit,1,2,1,2);

    gridLayout->addWidget(passwordLabel,2,1,1,1);
    gridLayout->addWidget(passwordEdit,2,2,1,2);

    gridLayout->addWidget(confirmPasswordLabel,3,1,1,1);
    gridLayout->addWidget(confirmPassEdit,3,2,1,2);

    gridLayout->addWidget(confirmButt,4,2,1,2);

    gridLayout->setRowStretch(5,1);
    gridLayout->setColumnStretch(4,1);

    mainWidget->setLayout(gridLayout);

    contentLayout->addWidget(mainWidget);
    contentWidget->setLayout(contentLayout);
}

RegistDialog::RegistDialog(QWidget *parent):
    d_ptr(new RegistDialogPrivate(this)),Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Sin up"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));
    setWindowsMoveable(false);

    setFixedSize(Constant::LOGIN_FIX_WIDTH,Constant::LOGIN_FIX_HEIGHT);

    ToolBar * bar = enableToolBar(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_MESSAGEBOX);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM_16));

        bar->setWindowTitle(windowTitle());
    }

qDebug()<<"Connect:"<<connect(MessDiapatch::instance(),SIGNAL(recvRegistResponse(ResponseRegister,RegistResponse)),
             this,SLOT(recvResponse(ResponseRegister,RegistResponse)),Qt::QueuedConnection);
}

RegistDialog::~RegistDialog()
{

}

void RegistDialog::respValidInfo()
{
    MQ_D(RegistDialog);

    QRegExp space("\\s+");

    if(d->nickNameEdit->text().size() <= 0 || d->nickNameEdit->text().contains(space))
    {
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Check nickname!"),RMessageBox::Yes);
        return;
    }

    if(d->passwordEdit->text().size() <=0 || d->passwordEdit->text().contains(space) || d->confirmPassEdit->text().size() <=0 ||
            d->confirmPassEdit->text().contains(space))
    {
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Check password!"),RMessageBox::Yes);
        return;
    }

    if(d->passwordEdit->text() != d->confirmPassEdit->text())
    {
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Check password!"),RMessageBox::Yes);
        return;
    }

    RegistRequest * request = new RegistRequest;

    request->nickName = d->nickNameEdit->text();
    request->password = RUtil::MD5(d->passwordEdit->text());

    QString ip = "127.0.0.1";
    unsigned short port = 8023;

    if(!RSingleton<NetConnector>::instance()->connect())
    {
        RLOG_ERROR("Connect to server %s:%d error!",ip.toLocal8Bit().data(),port);
        RMessageBox::warning(this,QObject::tr("Warning"),QObject::tr("Connect to server error!"),RMessageBox::Yes);
        return;
    }

    RSingleton<MsgWrap>::instance()->handleMsg(request);
}

void RegistDialog::recvResponse(ResponseRegister status, RegistResponse resp)
{
    if(status == REGISTER_SUCCESS)
    {
        RMessageBox::information(this,tr("Information"),tr("Registered successfully! Remember Id:【%1】").arg(resp.accountId),RMessageBox::Yes);
    }
    else
    {
        RMessageBox::warning(this,tr("Warning"),tr("Registration failed!"),RMessageBox::Yes);
    }
}

