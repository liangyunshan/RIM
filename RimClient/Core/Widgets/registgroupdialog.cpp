#include "registgroupdialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QDebug>

#include "datastruct.h"
#include "global.h"
#include "head.h"
#include "Util/imagemanager.h"
#include "Util/rutil.h"
#include "rsingleton.h"
#include "widget/rbutton.h"
#include "widget/rmessagebox.h"
#include "user/user.h"
#include "Network/msgwrap/wrapfactory.h"

#include "protocoldata.h"
using namespace ProtocolType;

class RegistGroupDialogPrivate : public GlobalData<RegistGroupDialog>
{
    Q_DECLARE_PUBLIC(RegistGroupDialog)

private:
    RegistGroupDialogPrivate(RegistGroupDialog * ptr):q_ptr(ptr)
    {
        initWidget();
    }

    void initWidget();

    QWidget * contentWidget;

    QLineEdit * groupNameEdit;              //群名称
    QLineEdit * groupDescEdit;              //群描述
    QLineEdit * groupLabelEdit;             //群描述

    QButtonGroup * visibleGroup;            //群是否可见(默认可见)
    QButtonGroup * validateGroup;           //群是否验证(默认不需要验证)

    QComboBox * questionCombox;             //验证问题(提供几个选项，同时支持自定义问题)
    QLineEdit * answerEdit;               //验证问题答案

    RButton * createButton;
    RButton * closeButton;

    RegistGroupDialog * q_ptr;
};

void RegistGroupDialogPrivate::initWidget()
{
    contentWidget = new QWidget();
    contentWidget->setObjectName("Widget_ContentWidget");

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    QWidget * bodyWidget = new QWidget();

    //名称
    QLabel * nameLabel = new QLabel(bodyWidget);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setText(QObject::tr("Group name:"));

    groupNameEdit = new QLineEdit(bodyWidget);
    groupNameEdit->setFixedWidth(210);

    //描述
    QLabel * descLabel = new QLabel(bodyWidget);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setText(QObject::tr("Group desc:"));

    groupDescEdit = new QLineEdit(bodyWidget);
    groupDescEdit->setFixedWidth(210);

    //标签
    QLabel * labelLabel = new QLabel(bodyWidget);
    labelLabel->setAlignment(Qt::AlignCenter);
    labelLabel->setText(QObject::tr("Group label:"));

    groupLabelEdit = new QLineEdit(bodyWidget);
    groupLabelEdit->setFixedWidth(210);

    //可见性
    QLabel * visibleLabel = new QLabel(bodyWidget);
    visibleLabel->setAlignment(Qt::AlignCenter);
    visibleLabel->setText(QObject::tr("Search visible:"));

    visibleGroup = new QButtonGroup;

    QRadioButton * visibleButt = new QRadioButton;
    visibleButt->setText(QObject::tr("visible"));
    visibleButt->setChecked(true);

    QRadioButton * invisibleButt = new QRadioButton;
    invisibleButt->setText(QObject::tr("invisible"));

    QWidget * visibleWidget = new QWidget();

    QHBoxLayout * visbleLayout = new QHBoxLayout;
    visbleLayout->addWidget(visibleButt);
    visbleLayout->addWidget(invisibleButt);
    visbleLayout->addStretch(1);
    visbleLayout->setContentsMargins(0,0,0,0);
    visibleWidget->setLayout(visbleLayout);

    visibleGroup->addButton(visibleButt,SEARCH_VISIBLE);
    visibleGroup->addButton(invisibleButt,SEARCH_INVISIBLE);

    //验证
    QLabel * validateLabel = new QLabel(bodyWidget);
    validateLabel->setAlignment(Qt::AlignCenter);
    validateLabel->setText(QObject::tr("Validate:"));

    validateGroup = new QButtonGroup;

    QRadioButton * noValidateButt = new QRadioButton;
    noValidateButt->setText(QObject::tr("no validate"));
    noValidateButt->setChecked(true);

    QRadioButton * validateButt = new QRadioButton;
    validateButt->setText(QObject::tr("validate"));

    //密码问题输入区域
    QWidget * validateWidget = new QWidget();

    QHBoxLayout * validateLayout = new QHBoxLayout;
    validateLayout->addWidget(validateButt);
    validateLayout->addWidget(noValidateButt);
    validateLayout->addStretch(1);

    QWidget * validateInputWidget = new QWidget();
    validateInputWidget->setVisible(false);

    questionCombox = new QComboBox;
    questionCombox->setEditable(true);
    QStringList questionList;
    questionList<<QStringLiteral("我的手机号码?")
               <<QStringLiteral("我的真实姓名?")
              <<QStringLiteral("我的公司名称?");
    questionCombox->addItems(questionList);

    QLabel * questionLabel = new QLabel();
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setText(QObject::tr("Question:"));

    QLabel * answerLabel = new QLabel();
    answerLabel->setAlignment(Qt::AlignCenter);
    answerLabel->setText(QObject::tr("Answer:"));

    answerEdit = new QLineEdit();

    QGridLayout * questionLayout = new QGridLayout;
    questionLayout->addWidget(questionLabel,0,0,1,1);
    questionLayout->addWidget(questionCombox,0,1,1,2);
    questionLayout->addWidget(answerLabel,1,0,1,1);
    questionLayout->addWidget(answerEdit,1,1,1,2);

    validateInputWidget->setLayout(questionLayout);

    QObject::connect(validateButt,SIGNAL(toggled(bool)),validateInputWidget,SLOT(setVisible(bool)));
    QObject::connect(validateButt,SIGNAL(toggled(bool)),q_ptr,SLOT(respValidateChanged(bool)));

    QVBoxLayout * vlayout = new QVBoxLayout;
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->addLayout(validateLayout);
    vlayout->addWidget(validateInputWidget);
    validateWidget->setLayout(vlayout);

    validateGroup->addButton(validateButt,JOIN_VALIDATE);
    validateGroup->addButton(noValidateButt,JOIN_NOVALIDATE);

    QGridLayout * gridLayout = new QGridLayout;
    gridLayout->setVerticalSpacing(10);
    gridLayout->setColumnStretch(0,1);
    gridLayout->setColumnStretch(4,1);

    gridLayout->addWidget(nameLabel,0,1,1,1);
    gridLayout->addWidget(groupNameEdit,0,2,1,2);

    gridLayout->addWidget(descLabel,1,1,1,1);
    gridLayout->addWidget(groupDescEdit,1,2,1,2);

    gridLayout->addWidget(labelLabel,2,1,1,1);
    gridLayout->addWidget(groupLabelEdit,2,2,1,2);

    gridLayout->addWidget(visibleLabel,3,1,1,1);
    gridLayout->addWidget(visibleWidget,3,2,1,2);

    gridLayout->addWidget(validateLabel,4,1,1,1);
    gridLayout->addWidget(validateWidget,4,2,1,2);

    gridLayout->setRowStretch(5,1);
    bodyWidget->setLayout(gridLayout);

    QWidget * bottomWidget = new QWidget(contentWidget);

    createButton = new RButton(bottomWidget);
    createButton->setText(QObject::tr("Regist"));

    QObject::connect(createButton,SIGNAL(pressed()),q_ptr,SLOT(prepareRegist()));

    closeButton = new RButton(bottomWidget);
    closeButton->setText(QObject::tr("Close"));
    QObject::connect(closeButton,SIGNAL(pressed()),q_ptr,SLOT(close()));

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,10,10);
    layout->addStretch(1);
    layout->addWidget(createButton);
    layout->addWidget(closeButton);

    bottomWidget->setLayout(layout);

    QVBoxLayout * bodyLayout = new QVBoxLayout;
    bodyLayout->setContentsMargins(0,0,0,0);
    bodyLayout->addWidget(bodyWidget);
    bodyLayout->addWidget(bottomWidget);

    contentWidget->setLayout(bodyLayout);

    QObject::connect(groupNameEdit,SIGNAL(textChanged(QString)),q_ptr,SLOT(validateInput(QString)));
    QObject::connect(groupDescEdit,SIGNAL(textChanged(QString)),q_ptr,SLOT(validateInput(QString)));
    QObject::connect(groupLabelEdit,SIGNAL(textChanged(QString)),q_ptr,SLOT(validateInput(QString)));
    QObject::connect(answerEdit,SIGNAL(textChanged(QString)),q_ptr,SLOT(validateInput(QString)));
    QObject::connect(questionCombox->lineEdit(),SIGNAL(textChanged(QString)),q_ptr,SLOT(validateInput(QString)));

    q_ptr->setContentWidget(contentWidget);
}

#define REGIST_GROUP_BASIC_WIDTH 380
#define REGIST_GROUP_BASIC_HEIGHT 400

RegistGroupDialog::RegistGroupDialog(QWidget *parent):
    Widget(parent),d_ptr(new RegistGroupDialogPrivate(this))
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));
    setToolBarMoveable(true);

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowTitle(tr("Create Group"));
    }

    setFixedSize(REGIST_GROUP_BASIC_WIDTH,REGIST_GROUP_BASIC_HEIGHT);
    QSize  screenSize = RUtil::screenSize();
    move((screenSize.width() - width())/2,(screenSize.height() - height())/2);

    validateInput("");
}

void RegistGroupDialog::prepareRegist()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(RegistGroupDialog);

    SearchVisible visible = (SearchVisible)d->visibleGroup->checkedId();
    JoinValidate validate = (JoinValidate)d->validateGroup->checkedId();

    RegistGroupRequest * request = new RegistGroupRequest;
    request->groupName = d->groupNameEdit->text();
    request->groupDesc = d->groupDescEdit->text();
    request->groupLabel = d->groupLabelEdit->text();
    request->searchVisible = visible == SEARCH_VISIBLE?true:false;
    request->validateAble = validate == JOIN_VALIDATE?true:false;
    request->validateQuestion = d->questionCombox->currentText();
    request->validateAnaswer = d->answerEdit->text();
    request->userId = G_User->BaseInfo().uuid;
    request->accountId = G_User->BaseInfo().accountId;

    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
}

void RegistGroupDialog::validateInput(QString)
{
    MQ_D(RegistGroupDialog);

    QString groupName = d->groupNameEdit->text();
    QString groupDesc = d->groupDescEdit->text();
    QString groupLabel = d->groupLabelEdit->text();

    if(groupName.size() <= 0 || groupDesc.size() <= 0 || groupLabel.size() <= 0){
        d->createButton->setEnabled(false);
    }else{
        JoinValidate validate = (JoinValidate)d->validateGroup->checkedId();
        QString question,answer;
        if(validate == JOIN_VALIDATE){
            question = d->questionCombox->currentText();
            answer = d->answerEdit->text();

            if(question.size() <= 0 || answer.size() <= 0){
                d->createButton->setEnabled(false);
                repolish(d->createButton);
                return;
            }
        }
        d->createButton->setEnabled(true);
    }
    repolish(d->createButton);
}

void RegistGroupDialog::respValidateChanged(bool flag)
{
    Q_UNUSED(flag);
    validateInput("");
}

void RegistGroupDialog::recvRegistResult(bool flag)
{
    if(flag){
        clearInput();
        RMessageBox::information(this,tr("infomation"),tr("Regist group success!"),RMessageBox::Yes);
    }else{
        RMessageBox::information(this,tr("warning"),tr("Regist group failed!"),RMessageBox::Yes);
    }
}

void RegistGroupDialog::clearInput()
{
    MQ_D(RegistGroupDialog);

    d->groupNameEdit->clear();
    d->groupDescEdit->clear();
    d->groupLabelEdit->clear();
    d->questionCombox->setCurrentIndex(0);
    d->answerEdit->clear();
}
