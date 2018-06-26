#include "systemnotifyview.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "head.h"
#include "../protocol/datastruct.h"
#include "constants.h"
#include "global.h"
#include "toolbar.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "Util/rutil.h"
#include "widget/rbutton.h"
#include "Network/msgwrap/wrapfactory.h"
#include "widget/rlabel.h"
#include "user/user.h"
#include "user/userfriendcontainer.h"
#include "widget/rmessagebox.h"

#define NOTIFY_VIEW_WIDTH 380
#define NOTIFY_VIEW_HEIGHT 260

class SystemNotifyViewPrivate : public GlobalData<SystemNotifyView>
{
    Q_DECLARE_PUBLIC(SystemNotifyView)
private:
    SystemNotifyViewPrivate(SystemNotifyView * q):q_ptr(q)
    {
        initWidget();
    }

    void initWidget();
    void setType(ResponseFriendApply type);

private:
    SystemNotifyView * q_ptr;

    QWidget * toolWidget;

    RIconLabel * iconLabel;
    QLabel * nickNameLabel;
    QLabel * accountLabel;

    QLabel * applyIconLabel;                    //请求结果图标
    QLabel * tipLabel;                          //请求信息

    RButton * agreeButt;
    RButton * refuseButt;
    RButton * ignoreButt;
    RButton * chatButt;
    RButton * reRequestButt;

    NotifyInfo notifyInfo;
};

void SystemNotifyViewPrivate::initWidget()
{
    QWidget * contentWidget = new QWidget();
    contentWidget->setObjectName("Widget_ContentWidget");

    QWidget * widget = new QWidget(contentWidget);
    widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    iconLabel = new RIconLabel(widget);
    iconLabel->setTransparency(true);
    iconLabel->setEnterCursorChanged(false);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon());
    iconLabel->setFixedSize(64,64);

    nickNameLabel = new QLabel(widget);
    nickNameLabel->setFixedHeight(20);
    nickNameLabel->setAlignment(Qt::AlignCenter);

    accountLabel = new QLabel(widget);
    accountLabel->setFixedHeight(20);
    accountLabel->setAlignment(Qt::AlignCenter);

    applyIconLabel = new QLabel(widget);
    tipLabel = new QLabel(widget);

    QGridLayout * gridLayout = new QGridLayout;
    gridLayout->setRowStretch(0,1);
    gridLayout->setColumnStretch(0,1);

    gridLayout->addWidget(iconLabel,1,1,2,2);
    gridLayout->addWidget(nickNameLabel,3,1,1,2);
    gridLayout->addWidget(accountLabel,4,1,1,2);

    gridLayout->setColumnStretch(3,1);

    gridLayout->addWidget(applyIconLabel,1,4,2,1);
    gridLayout->addWidget(tipLabel,1,5,2,4);

    gridLayout->setRowStretch(5,1);
    gridLayout->setColumnStretch(10,1);

    widget->setLayout(gridLayout);

    toolWidget = new QWidget(contentWidget);

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->addWidget(widget);
    contentLayout->addWidget(toolWidget);

    contentWidget->setLayout(contentLayout);
    q_ptr->setContentWidget(contentWidget);
}

/*!
 * @brief 根据类型设置页面显示
 * @param[in] type 好友操作类型
 * @return 无
 */
void SystemNotifyViewPrivate::setType(ResponseFriendApply type)
{
    QHBoxLayout * layout = nullptr;
    if(toolWidget->layout()){
        layout = dynamic_cast<QHBoxLayout *>(toolWidget->layout());
        if(layout){
            for(int i = layout->count() - 1; i >= 0 ;i--){
                QLayoutItem * item = layout->takeAt(i);
                if(item->widget()){
                    delete item->widget();
                }
            }
        }
    }
    if(layout == nullptr){
        layout = new QHBoxLayout;
        toolWidget->setLayout(layout);
    }

    layout->setContentsMargins(0,0,10,10);
    layout->addStretch(1);
    switch(type)
    {
        case FRIEND_REQUEST:
                {
                    applyIconLabel->setPixmap(RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_STAR,ImageManager::ICON_32));
                    if(notifyInfo.stype == OperatePerson)
                        tipLabel->setText(QObject::tr("Request to add as a friend"));
                    else if(notifyInfo.stype == OperateGroup)
                        tipLabel->setText(QObject::tr("Apply for Group %1(%2)").arg(notifyInfo.chatName,notifyInfo.chatId));

                    agreeButt = new RButton(toolWidget);
                    agreeButt->setText(QObject::tr("Agree"));

                    refuseButt = new RButton(toolWidget);
                    refuseButt->setText(QObject::tr("Refuse"));

                    ignoreButt = new RButton(toolWidget);
                    ignoreButt->setText(QObject::tr("Ignore"));

                    QObject::connect(agreeButt,SIGNAL(pressed()),q_ptr,SLOT(respAgree()));
                    QObject::connect(refuseButt,SIGNAL(pressed()),q_ptr,SLOT(respRefuse()));
                    QObject::connect(ignoreButt,SIGNAL(pressed()),q_ptr,SLOT(respIgnore()));

                    layout->addWidget(agreeButt);
                    layout->addWidget(refuseButt);
                    layout->addWidget(ignoreButt);
                }
                break;
        case FRIEND_AGREE:
                {
                    applyIconLabel->setPixmap(RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_SUCCESS,ImageManager::ICON_32));
                    if(notifyInfo.stype == OperatePerson)
                        tipLabel->setText(QObject::tr("Agree with the request"));
                    else if(notifyInfo.stype == OperateGroup)
                        tipLabel->setText(QObject::tr("Agree with the apply for group %1(%2)").arg(notifyInfo.chatName,notifyInfo.chatId));

                    chatButt = new RButton(toolWidget);
                    chatButt->setText(QObject::tr("Chat"));

                    QObject::connect(chatButt,SIGNAL(pressed()),q_ptr,SLOT(respChat()));
                    layout->addWidget(chatButt);
                }
                break;
        case FRIEND_REFUSE:
                {
                    applyIconLabel->setPixmap(RSingleton<ImageManager>::instance()->getIcon(ImageManager::ICON_ERROR,ImageManager::ICON_32));
                    if(notifyInfo.stype == OperatePerson)
                        tipLabel->setText(QObject::tr("Refuse the request"));
                    else if(notifyInfo.stype == OperateGroup)
                        tipLabel->setText(QObject::tr("Refuse the apply for group %1(%2)").arg(notifyInfo.chatName,notifyInfo.chatId));

                    reRequestButt = new RButton(toolWidget);
                    reRequestButt->setText(QObject::tr("ReRequest"));

                    QObject::connect(reRequestButt,SIGNAL(pressed()),q_ptr,SLOT(respReRequest()));
                    layout->addWidget(reRequestButt);
                }
                break;
        default:
            break;
    }
}

SystemNotifyView::SystemNotifyView(QWidget *parent):
    d_ptr(new SystemNotifyViewPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Notify"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));
    setToolBarMoveable(true);

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));
        bar->setWindowTitle(windowTitle());
    }

    setFixedSize(NOTIFY_VIEW_WIDTH,NOTIFY_VIEW_HEIGHT);
}

SystemNotifyView::~SystemNotifyView()
{

}

void SystemNotifyView::setNotifyType(ResponseFriendApply type)
{
    MQ_D(SystemNotifyView);
    d->setType(type);
}

void SystemNotifyView::setNotifyInfo(NotifyInfo info)
{
    MQ_D(SystemNotifyView);
    d->notifyInfo = info;

    d->accountLabel->setText(info.accountId);
    d->nickNameLabel->setText(info.nickName);
}

void SystemNotifyView::resizeEvent(QResizeEvent *)
{
    QSize size = RUtil::screenSize();
    move((size.width() - NOTIFY_VIEW_WIDTH)/2,(size.height() - NOTIFY_VIEW_HEIGHT)/2);
}

void SystemNotifyView::respAgree(){
    MQ_D(SystemNotifyView);

    if(d->notifyInfo.stype == OperatePerson){
        if(RSingleton<UserFriendContainer>::instance()->containUser(d->notifyInfo.accountId)){
            d->setType(FRIEND_AGREE);
            return;
        }
    }
    sendResponse(FRIEND_AGREE);

    close();
}

void SystemNotifyView::respRefuse(){
    MQ_D(SystemNotifyView);
    if(d->notifyInfo.stype == OperatePerson){
        if(RSingleton<UserFriendContainer>::instance()->containUser(d->notifyInfo.accountId)){
            d->setType(FRIEND_AGREE);
            return;
        }
    }
    sendResponse(FRIEND_REFUSE);

    close();
}

void SystemNotifyView::respIgnore(){
    close();
}

void SystemNotifyView::respChat()
{
    MQ_D(SystemNotifyView);
    emit chatWidth(d->notifyInfo.accountId);
    close();
}

void SystemNotifyView::respReRequest()
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(SystemNotifyView);
    AddFriendRequest * request = new AddFriendRequest;
    request->stype = d->notifyInfo.stype;
    request->accountId = G_User->BaseInfo().accountId;
    if(d->notifyInfo.stype == OperatePerson)
        request->operateId = d->notifyInfo.accountId;
    else if(d->notifyInfo.stype == OperateGroup)
        request->operateId = d->notifyInfo.chatId;
    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
}

void SystemNotifyView::sendResponse(ResponseFriendApply result)
{
    R_CHECK_ONLINE;
    R_CHECK_LOGIN;
    MQ_D(SystemNotifyView);
    OperateFriendRequest * request = new OperateFriendRequest;
    request->type = FRIEND_APPLY;
    request->stype = d->notifyInfo.stype;
    request->result = (int)result;
    request->accountId = G_User->BaseInfo().accountId;
    request->operateId = d->notifyInfo.accountId;
    if(d->notifyInfo.stype == OperateGroup){
        request->chatId = d->notifyInfo.chatId;
        request->chatName = d->notifyInfo.chatName;
    }

    RSingleton<WrapFactory>::instance()->getMsgWrap()->handleMsg(request);
}

