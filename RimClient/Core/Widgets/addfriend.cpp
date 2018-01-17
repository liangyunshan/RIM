#include "addfriend.h"

#include <QRadioButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>

#include "head.h"
#include "constants.h"
#include "datastruct.h"
#include "widget/rbutton.h"
#include "Util/rutil.h"
#include "widget/riconlabel.h"

#include "Util/imagemanager.h"
#include "rsingleton.h"

#define ADD_FRIEND_WIDTH 500
#define ADD_FRIEND_HEIGHT 240

class AddFriendPrivate : public GlobalData<AddFriend>
{
    Q_DECLARE_PUBLIC(AddFriend)

private:
    AddFriendPrivate(AddFriend * q):q_ptr(q),
        LabelWidth(200)
    {
        initWidget();
    }

    void initWidget();

private:
    AddFriend * q_ptr;

    const int LabelWidth;

    QStackedWidget * stackedWidget;

    QWidget * lookupWidget;
    QWidget * resultWidget;

    QLabel * searchTip;
    QRadioButton * person_Radio;
    QRadioButton * group_Radio;

    QLabel * statusLabel;

    QLineEdit * inputEdit;
    RButton * searchButt;

    RIconLabel * userIconLabel;
    QLabel * accountLabel;
    QLabel * userNickNameLabel;
    RButton * addRelationButt;
    RButton * reLookupButt;
};

void AddFriendPrivate::initWidget()
{
    stackedWidget = new QStackedWidget(q_ptr);
    stackedWidget->setObjectName("Widget_ContentWidget");

    /**************查询页面***************/
    lookupWidget = new QWidget();

    QWidget * container = new QWidget(lookupWidget);
    QHBoxLayout * containLayout = new QHBoxLayout;
    containLayout->setContentsMargins(0,0,0,0);
    containLayout->setSpacing(6);

    searchTip = new QLabel(lookupWidget);
    searchTip->setText(QObject::tr("Lookup type"));

    person_Radio = new QRadioButton(lookupWidget);
    person_Radio->setText(QObject::tr("person"));
    person_Radio->setChecked(true);

    group_Radio = new QRadioButton(lookupWidget);
    group_Radio->setText(QObject::tr("group"));

    searchButt = new RButton(lookupWidget);
    searchButt->setText(QObject::tr("Search"));
    QObject::connect(searchButt,SIGNAL(pressed()),q_ptr,SLOT(startSearch()));

    containLayout->addWidget(searchTip);
    containLayout->addSpacing(4);
    containLayout->addWidget(person_Radio);
    containLayout->addWidget(group_Radio);
    container->setLayout(containLayout);

    inputEdit = new QLineEdit(lookupWidget);
    inputEdit->setFixedSize(LabelWidth,25);
    inputEdit->setPlaceholderText(QObject::tr("Input number"));

    statusLabel = new QLabel(lookupWidget);
    statusLabel->setMinimumHeight(Constant::ITEM_FIX_HEIGHT);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setVisible(false);

    QGridLayout * layout = new QGridLayout;
    layout->setRowStretch(0,1);
    layout->setColumnStretch(0,1);
    layout->addWidget(inputEdit,1,1,1,2);
    layout->addWidget(searchButt,1,4,1,1);
    layout->setRowStretch(2,1);

    layout->addWidget(container,3,1,1,4);
    layout->setRowStretch(4,3);
    layout->addWidget(statusLabel,5,1,1,4);
    layout->setColumnStretch(5,1);

    lookupWidget->setLayout(layout);

    /**************结果页面***************/
    resultWidget = new QWidget();

    userIconLabel = new RIconLabel(resultWidget);
    userIconLabel->setFixedSize(64,64);
    userIconLabel->setTransparency(true);
    userIconLabel->setCorner();

    accountLabel = new QLabel(resultWidget);
    accountLabel->setObjectName("Add_Friend_Account_Label");
    accountLabel->setMinimumWidth(LabelWidth);

    userNickNameLabel = new QLabel(resultWidget);
    userNickNameLabel->setMinimumWidth(LabelWidth);

    reLookupButt = new RButton(resultWidget);
    reLookupButt->setText(QObject::tr("Redo"));

    addRelationButt = new RButton(resultWidget);
    addRelationButt->setText(QObject::tr("Add"));

    QGridLayout * resultLayout = new QGridLayout;
    resultLayout->setRowStretch(0,1);
    resultLayout->setColumnStretch(0,1);
    resultLayout->addWidget(userIconLabel,1,1,2,1);
    resultLayout->addWidget(accountLabel,1,2,1,2);
    resultLayout->addWidget(userNickNameLabel,2,2,1,2);
    resultLayout->setColumnStretch(4,1);

    resultLayout->setRowStretch(4,1);
    resultLayout->addWidget(reLookupButt,5,2,1,1);
    resultLayout->addWidget(addRelationButt,5,3,1,1);
    resultLayout->setRowStretch(6,2);

    QObject::connect(reLookupButt,SIGNAL(pressed()),q_ptr,SLOT(reSearch()));

    resultWidget->setLayout(resultLayout);

    stackedWidget->addWidget(lookupWidget);
    stackedWidget->addWidget(resultWidget);

    q_ptr->setContentWidget(stackedWidget);
}

AddFriend::AddFriend(QWidget * parent):
    d_ptr(new AddFriendPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Lookup"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    ToolBar * bar = enableToolBar(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM_16));

        bar->setWindowTitle(tr("Lookup"));
    }
    setFixedSize(ADD_FRIEND_WIDTH,ADD_FRIEND_HEIGHT);
    QSize  screenSize = RUtil::screenSize();
    setGeometry((screenSize.width() - ADD_FRIEND_WIDTH)/2,screenSize.height()/2 - ADD_FRIEND_HEIGHT,ADD_FRIEND_WIDTH,ADD_FRIEND_HEIGHT);
}

AddFriend::~AddFriend()
{
    delete d_ptr;
}

void AddFriend::onMessage(MessageType)
{

}

void AddFriend::startSearch()
{
    MQ_D(AddFriend);
    d->statusLabel->setVisible(true);
    d->statusLabel->setText("正在查找...");
    d->userNickNameLabel->setText(QStringLiteral("Say Something"));
    d->userIconLabel->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon());

    d->accountLabel->setText("407859345");

    d->stackedWidget->setCurrentIndex(1);
}

void AddFriend::reSearch()
{
    MQ_D(AddFriend);

    d->userNickNameLabel->clear();
    d->accountLabel->clear();
    d->userIconLabel->setPixmap("");
    d->stackedWidget->setCurrentIndex(0);
    d->statusLabel->setVisible(false);
}


