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
#include "global.h"
#include "datastruct.h"
#include "widget/rbutton.h"
#include "Util/rutil.h"
#include "widget/rlabel.h"
#include "widget/rmessagebox.h"
#include "Util/imagemanager.h"
#include "rsingleton.h"
#include "Network/msgwrap.h"
#include "messdiapatch.h"
#include "toolbox/listbox.h"

#define ADD_FRIEND_WIDTH 500
#define ADD_FRIEND_HEIGHT 400

class AddFriendPrivate : public GlobalData<AddFriend>
{
    Q_DECLARE_PUBLIC(AddFriend)

private:
    AddFriendPrivate(AddFriend * q):q_ptr(q),
        LabelWidth(200)
    {
        initWidget();
    }

    void enableSearch(bool flag)
    {
        addRelationButt->setEnabled(flag);
        q_ptr->repolish(addRelationButt);
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

    ListBox * searchList;

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
    searchButt->setEnabled(false);
    q_ptr->repolish(searchButt);
    QObject::connect(searchButt,SIGNAL(pressed()),q_ptr,SLOT(startSearch()));

    containLayout->addWidget(searchTip);
    containLayout->addSpacing(4);
    containLayout->addWidget(person_Radio);
    containLayout->addWidget(group_Radio);
    container->setLayout(containLayout);

    inputEdit = new QLineEdit(lookupWidget);
    inputEdit->setFixedSize(LabelWidth,25);
    inputEdit->setPlaceholderText(QObject::tr("Input number or nickname"));
    QObject::connect(inputEdit,SIGNAL(textChanged(QString)),q_ptr,SLOT(validateInputState(QString)));

    statusLabel = new QLabel(lookupWidget);
    statusLabel->setMinimumHeight(Constant::ITEM_FIX_HEIGHT);
    statusLabel->setAlignment(Qt::AlignCenter);

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

    QWidget * searchWidget = new QWidget();

    searchList = new ListBox(searchWidget);
    QObject::connect(searchList,SIGNAL(currentItemChanged(ToolItem*)),q_ptr,SLOT(itemSelected(ToolItem *)));

    QHBoxLayout * searchLayout = new QHBoxLayout;
    searchLayout->setContentsMargins(0,10,0,0);
    searchLayout->addStretch(1);
    searchLayout->addWidget(searchList);
    searchLayout->setStretchFactor(searchList,4);
    searchLayout->addStretch(1);

    searchWidget->setLayout(searchLayout);

    QWidget * toolBox = new QWidget();
    QHBoxLayout * toolLayout = new QHBoxLayout;

    reLookupButt = new RButton(toolBox);
    reLookupButt->setText(QObject::tr("Redo"));

    addRelationButt = new RButton(toolBox);
    addRelationButt->setText(QObject::tr("Add"));
    enableSearch(false);

    toolLayout->setContentsMargins(0,0,10,10);
    toolLayout->addStretch(1);
    toolLayout->addWidget(reLookupButt);
    toolLayout->addWidget(addRelationButt);
    toolBox->setLayout(toolLayout);

    QVBoxLayout * resultLayout = new QVBoxLayout;
    resultLayout->setContentsMargins(0,0,0,0);
    resultLayout->addWidget(searchWidget);
    resultLayout->addWidget(toolBox);

    QObject::connect(reLookupButt,SIGNAL(pressed()),q_ptr,SLOT(reSearch()));
    QObject::connect(addRelationButt,SIGNAL(pressed()),q_ptr,SLOT(addFriend()));

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

    connect(MessDiapatch::instance(),SIGNAL(recvSearchFriendResponse(ResponseAddFriend,SearchFriendResponse)),this,
            SLOT(recvSearchFriendResponse(ResponseAddFriend,SearchFriendResponse)));
    connect(MessDiapatch::instance(),SIGNAL(recvAddFriendResponse(ResponseAddFriend)),this,SLOT(recvAddFriendResponse(ResponseAddFriend)));
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

    SearchFriendRequest * request = new SearchFriendRequest;
    request->accountOrNickName = d->inputEdit->text();
    request->stype = d->person_Radio->isChecked()?SearchPerson:SearchGroup;

    RSingleton<MsgWrap>::instance()->handleMsg(request);
    d->statusLabel->setText(tr("searching..."));

    enableInput(false);
}

void AddFriend::reSearch()
{
    MQ_D(AddFriend);

    enableInput(true);
    d->statusLabel->clear();
    d->stackedWidget->setCurrentIndex(0);
    d->searchList->clear();
}

void AddFriend::addFriend()
{
    MQ_D(AddFriend);
    if(d->searchList->selectedItem())
    {
        AddFriendRequest * request = new AddFriendRequest;
        request->stype = d->person_Radio->isChecked()?SearchPerson:SearchGroup;
        request->accountId = G_UserBaseInfo.accountId;
        request->friendId = d->searchList->selectedItem()->getName();
        RSingleton<MsgWrap>::instance()->handleMsg(request);
    }
}

void AddFriend::validateInputState(QString content)
{
    MQ_D(AddFriend);

    bool isCorrect = true;

    if(content.contains(QRegExp(Constant::Space_Reg)))
    {
        isCorrect = false;
        d->statusLabel->setText(tr("input contains space"));
    }

    if(isCorrect && content.length() <= 0)
    {
        isCorrect = false;
        d->statusLabel->setText(tr("empty input"));
    }

    if(isCorrect)
    {
        d->statusLabel->clear();
    }

    d->searchButt->setEnabled(isCorrect);

    repolish(d->searchButt);
}

void AddFriend::recvSearchFriendResponse(ResponseAddFriend status, SearchFriendResponse response)
{
    MQ_D(AddFriend);
    if(status == FIND_FRIEND_FOUND)
    {
        d->stackedWidget->setCurrentIndex(1);

        for(int i = 0; i < response.result.size(); i++)
        {
            ToolItem * item = new ToolItem(NULL);
            item->setName(response.result.at(i).accountId);
            item->setNickName(response.result.at(i).nickName);
            item->setDescInfo(response.result.at(i).signName);
            d->searchList->addItem(item);
        }
    }
    else
    {
         QString errorInfo;
         switch(status)
         {
              case FIND_FRIEND_NOT_FOUND:
                                         errorInfo = QObject::tr("No result");
                                         break;
              case FIND_FRIEND_FAILED:
              default:
                                        errorInfo = QObject::tr("Find failed");
                                        break;
         }
         enableInput(true);
         d->statusLabel->setText(errorInfo);
    }
}

void AddFriend::recvAddFriendResponse(ResponseAddFriend status)
{
    switch(status)
    {
        case ADD_FRIEND_SENDED:
                                RMessageBox::information(this,"information","Request send successfully",RMessageBox::Yes);
                                break;
        case ADD_FRIEND_SENDED_FAILED:
        default:
                                RMessageBox::warning(this,"warning","Request send failed",RMessageBox::Yes);
                                break;
    }
}

void AddFriend::itemSelected(ToolItem * item)
{
    MQ_D(AddFriend);
    if(item)
    {
        d->enableSearch(true);
    }
}

void AddFriend::enableInput(bool flag)
{
    MQ_D(AddFriend);
    d->inputEdit->setReadOnly(!flag);
    d->searchButt->setEnabled(flag);
    repolish(d->searchButt);
}


