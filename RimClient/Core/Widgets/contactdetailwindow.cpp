#include "contactdetailwindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "head.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "widget/rlabel.h"
#include "widget/rcombobox.h"
#include "Network/msgwrap.h"
#include "messdiapatch.h"

#define DETAIL_SHOW_WIDTH 380
#define DETAIL_SHOW_HEIGHT 600

class ContactDetailWindowPrivate : public GlobalData<ContactDetailWindow>
{
    Q_DECLARE_PUBLIC(ContactDetailWindow)

private:
    ContactDetailWindowPrivate(ContactDetailWindow * q):q_ptr(q)
    {
        initWidget();
        m_headPath = QString();
    }

    ContactDetailWindow * q_ptr;

    QWidget * contentWidget;

    QLabel * m_accountIcon_label;
    QLineEdit * m_account_edit;

    QLabel * m_nickIcon_label;
    QLabel * m_nickName_label;
    QLineEdit * m_nickName_edit;

    QLabel * m_remarkIcon_label;
    QLabel * m_remark_label;
    QLineEdit * m_remark_edit;
    QPushButton * m_modify_button;

    RIconLabel * m_head_label;
    QLineEdit * m_signName_edit;

    QLabel * m_groupIcon_label;
    QLabel * m_group_label;
    RComboBox * m_group_box;

    QLabel * m_infoIcon_label;
    QLineEdit * m_sex_edit;
    QLineEdit * m_birthDay_edit;

    QLabel * m_locationIcon_label;
    QLabel * m_location_label;
    QLineEdit * m_location_edit;

    QString m_headPath;

    void initWidget();
};

void ContactDetailWindowPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Widget_ContentWidget");

    const int Label_Width = 32;
    const int Label_Height = 25;
    const int Min_Edit_Width = 35;
    const int Max_Edit_Width = 110;

    m_accountIcon_label = new QLabel(contentWidget);
    m_accountIcon_label->setFixedSize(Label_Height,Label_Height);
    m_accountIcon_label->setStyleSheet("border-image:url(:/icon/resource/icon/Detail_Account.png);");

    m_account_edit = new QLineEdit(contentWidget);
    m_account_edit->setReadOnly(true);
    m_account_edit->setFrame(false);
    m_account_edit->setFixedHeight(Label_Height);
    m_account_edit->setMinimumWidth(Min_Edit_Width);
    m_account_edit->setMaximumWidth(Max_Edit_Width);

    m_nickIcon_label = new QLabel(contentWidget);
    m_nickIcon_label->setFixedSize(Label_Height,Label_Height);
    m_nickIcon_label->setStyleSheet("border-image:url(:/icon/resource/icon/Detail_Nick.png);");

    m_nickName_label = new QLabel(contentWidget);
    m_nickName_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_nickName_label->setFixedSize(Label_Width,Label_Height);
    m_nickName_label->setText(QObject::tr("Nick Name"));

    m_nickName_edit = new QLineEdit(contentWidget);
    m_nickName_edit->setReadOnly(true);

    m_nickName_edit->setFrame(false);

    m_nickName_edit->setMinimumWidth(Min_Edit_Width);
    m_nickName_edit->setMaximumWidth(Max_Edit_Width);

    m_remarkIcon_label = new QLabel(contentWidget);
    m_remarkIcon_label->setFixedSize(Label_Height,Label_Height);
    m_remarkIcon_label->setStyleSheet("border-image:url(:/icon/resource/icon/Detail_Remark.png);");

    m_remark_label = new QLabel(contentWidget);
    m_remark_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_remark_label->setFixedSize(Label_Width,Label_Height);
    m_remark_label->setText(QObject::tr("Remark Name"));

    m_remark_edit = new QLineEdit(contentWidget);
    m_remark_edit->setReadOnly(true);
    m_remark_edit->setFrame(false);
    m_remark_edit->adjustSize();
    QObject::connect(m_remark_edit,SIGNAL(editingFinished()),q_ptr,SLOT(remarkEditFinished()));

    m_modify_button = new QPushButton(contentWidget);
    m_modify_button->setFixedSize(Label_Height,Label_Height);
    m_modify_button->setStyleSheet("background-color:rgba(255, 255, 255, 0);border-image:url(:/icon/resource/icon/Detail_EditRemarkl.png);");
    QObject::connect(m_modify_button,SIGNAL(clicked()),q_ptr,SLOT(modifyRemark()));

    m_head_label = new RIconLabel(contentWidget);
    m_head_label->setToolTip(QObject::tr("Clicked To See Head"));
    m_head_label->setTransparency(true);
    QObject::connect(m_head_label,SIGNAL(mousePressed()),q_ptr,SLOT(showHead()));

    m_signName_edit = new QLineEdit(contentWidget);
    m_signName_edit->setReadOnly(true);
    m_signName_edit->setFrame(false);
    m_signName_edit->setMinimumWidth(Min_Edit_Width);
    m_signName_edit->setMaximumWidth(Max_Edit_Width);

    m_groupIcon_label = new QLabel(contentWidget);
    m_groupIcon_label->setFixedSize(Label_Height,Label_Height);
    m_groupIcon_label->setStyleSheet("border-image:url(:/icon/resource/icon/Detail_Group.png);");

    m_group_label = new QLabel(contentWidget);
    m_group_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_group_label->setFixedSize(Label_Width,Label_Height);
    m_group_label->setText(QObject::tr("Group"));

    m_group_box = new RComboBox(contentWidget);
    QObject::connect(m_group_box,SIGNAL(currentIndexChanged(int)),q_ptr,SLOT(modifyGroup(int)));

    m_infoIcon_label = new QLabel(contentWidget);
    m_infoIcon_label->setFixedSize(Label_Height,Label_Height);
    m_infoIcon_label->setStyleSheet("border-image:url(:/icon/resource/icon/Detail_BaseInfo.png);");

    m_sex_edit = new QLineEdit(contentWidget);
    m_sex_edit->setReadOnly(true);
    m_sex_edit->setFrame(false);
    m_sex_edit->setFixedHeight(Label_Height);
    m_sex_edit->setMinimumWidth(Min_Edit_Width);
    m_sex_edit->setMaximumWidth(Max_Edit_Width);

    m_birthDay_edit = new QLineEdit(contentWidget);
    m_birthDay_edit->setReadOnly(true);
    m_birthDay_edit->setFrame(false);
    m_birthDay_edit->setFixedHeight(Label_Height);
    m_birthDay_edit->setMinimumWidth(Min_Edit_Width);
    m_birthDay_edit->setMaximumWidth(Max_Edit_Width);

    m_locationIcon_label = new QLabel(contentWidget);
    m_locationIcon_label->setFixedSize(Label_Height,Label_Height);
    m_locationIcon_label->setStyleSheet("border-image:url(:/icon/resource/icon/Detail_Address.png);");

    m_location_label = new QLabel(contentWidget);
    m_location_label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_location_label->setFixedSize(Label_Width,Label_Height);
    m_location_label->setText(QObject::tr("Address"));

    m_location_edit = new QLineEdit(contentWidget);
    m_location_edit->setReadOnly(true);
    m_location_edit->setFrame(false);
    m_location_edit->setFixedHeight(Label_Height);
    m_location_edit->setMinimumWidth(Min_Edit_Width);
    m_location_edit->setMaximumWidth(Max_Edit_Width);

    QHBoxLayout * accountLayout = new QHBoxLayout;
    accountLayout->setContentsMargins(0,0,0,0);
    accountLayout->addWidget(m_accountIcon_label);
    accountLayout->addWidget(m_account_edit);
    accountLayout->addStretch(1);

    QHBoxLayout * nickNameLayout = new QHBoxLayout;
    nickNameLayout->setContentsMargins(0,0,0,0);
    nickNameLayout->addWidget(m_nickIcon_label);
    nickNameLayout->addWidget(m_nickName_label);
    nickNameLayout->addWidget(m_nickName_edit);
    nickNameLayout->addStretch(1);

    QHBoxLayout * remarkNameLayout = new QHBoxLayout;
    remarkNameLayout->setContentsMargins(0,0,0,0);
    remarkNameLayout->addWidget(m_remarkIcon_label);
    remarkNameLayout->addWidget(m_remark_label);
    remarkNameLayout->addWidget(m_remark_edit);
    remarkNameLayout->addWidget(m_modify_button);
    remarkNameLayout->addStretch(1);

    QVBoxLayout * leftLayout = new QVBoxLayout;
    leftLayout->addLayout(accountLayout);
    leftLayout->addLayout(nickNameLayout);
    leftLayout->addLayout(remarkNameLayout);

    QVBoxLayout * headSignLayout = new QVBoxLayout;
    headSignLayout->setContentsMargins(0,0,0,0);
    headSignLayout->addWidget(m_head_label);
    headSignLayout->addWidget(m_signName_edit);

    QHBoxLayout * topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(0,0,0,0);
    topLayout->addLayout(leftLayout);
    topLayout->addStretch(1);
    topLayout->addLayout(headSignLayout);

    QHBoxLayout * groupLayout = new QHBoxLayout;
    groupLayout->addWidget(m_groupIcon_label);
    groupLayout->addWidget(m_group_label);
    groupLayout->addWidget(m_group_box);
    groupLayout->addStretch(1);

    QHBoxLayout * informationLayout = new QHBoxLayout;
    informationLayout->addWidget(m_infoIcon_label);
    informationLayout->addWidget(m_sex_edit);
    informationLayout->addWidget(m_birthDay_edit);
    informationLayout->addStretch(1);

    QHBoxLayout * locationLayout = new QHBoxLayout;
    locationLayout->addWidget(m_locationIcon_label);
    locationLayout->addWidget(m_location_label);
    locationLayout->addWidget(m_location_edit);
    locationLayout->addStretch(1);

    QVBoxLayout * layout = new QVBoxLayout;
    layout->addLayout(topLayout);
    layout->addLayout(groupLayout);
    layout->addLayout(informationLayout);
    layout->addLayout(locationLayout);
    layout->addStretch(1);

    contentWidget->setLayout(layout);
    q_ptr->setContentWidget(contentWidget);
}

ContactDetailWindow::ContactDetailWindow(QWidget *parent):
    d_ptr(new ContactDetailWindowPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Contact Person Detail"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));

        bar->setWindowTitle(tr("Contact Person Detail"));
    }

    RSingleton<Subject>::instance()->attach(this);

    setFixedSize(DETAIL_SHOW_WIDTH,DETAIL_SHOW_HEIGHT);
    QSize  screenSize = RUtil::screenSize();
    setGeometry((screenSize.width() - DETAIL_SHOW_WIDTH)/2,(screenSize.height() - DETAIL_SHOW_HEIGHT)/2,DETAIL_SHOW_WIDTH,DETAIL_SHOW_HEIGHT);

    connect(MessDiapatch::instance(),SIGNAL(recvUpdateBaseInfoResponse(ResponseUpdateUser,UpdateBaseInfoResponse)),this,SLOT(recvContactDetailResponse(ResponseUpdateUser,UpdateBaseInfoResponse)));

}

ContactDetailWindow::~ContactDetailWindow()
{
    RSingleton<Subject>::instance()->detach(this);
    delete d_ptr;
}

void ContactDetailWindow::onMessage(MessageType type)
{
    Q_UNUSED(type);
}

/*!
     * @brief 资料显示界面中显示联系人的基本信息
     * @param[in] info：const SimpleUserInfo &
     * @return 无
     */
void ContactDetailWindow::setContactDetail(const SimpleUserInfo &info)
{
    MQ_D(ContactDetailWindow);
    d->m_account_edit->setText(info.accountId);
    d->m_nickName_edit->setText(info.nickName);
    d->m_remark_edit->setText(info.remarks);
    d->m_remark_edit->adjustSize();
    d->m_headPath = RSingleton<ImageManager>::instance()->getSystemUserIcon(info.face);
    d->m_head_label->setPixmap(d->m_headPath);
    d->m_signName_edit->setText(info.signName);
    UpdateBaseInfoRequest * request = new UpdateBaseInfoRequest;
    request->baseInfo.accountId = info.accountId;
    request->requestType = REQUEST_CONTACT_DETAIL;
    RSingleton<MsgWrap>::instance()->handleMsg(request);
}

/*!
     * @brief 在分组下拉框中填充当前全部的分组名称并设置联系人所属分组
     * @param[in] groups:QStringList,全部分组名称;current:int,当前分组索引值
     * @return 无
     */
void ContactDetailWindow::setGroups(QStringList groups, int current)
{
    MQ_D(ContactDetailWindow);
    d->m_group_box->addItems(groups);
    if(current<d->m_group_box->count())
    {
        d->m_group_box->setCurrentIndex(current);
    }
}

/*!
     * @brief 资料显示界面中修改备注名称响应槽
     * @param[in]
     * @return 无
     */
void ContactDetailWindow::modifyRemark()
{
    MQ_D(ContactDetailWindow);
    d->m_remark_edit->setReadOnly(false);
    d->m_remark_edit->selectAll();
    d->m_remark_edit->setFocus();
}

/*!
     * @brief 资料显示界面中修改所属分组响应槽
     * @param[in]
     * @return 无
     */
void ContactDetailWindow::modifyGroup(int)
{

}

/*!
     * @brief 点击头像大图查看头像
     * @details 使用m_headPath来创建QPixmap并通过QLabel显示
     * @param[in]
     * @return 无
     */
void ContactDetailWindow::showHead()
{
    MQ_D(ContactDetailWindow);
    if(d->m_headPath.size() == 0)
    {
        return;
    }
}

/*!
     * @brief 收到服务器回复的联系人详细信息
     * @param[in] status:ResponseUpdateUser,回复状态;response:UpdateBaseInfoResponse,回复内容
     * @return 无
     */
void ContactDetailWindow::recvContactDetailResponse(ResponseUpdateUser status,UpdateBaseInfoResponse response)
{
    MQ_D(ContactDetailWindow);
    if(status == UPDATE_USER_SUCCESS && response.reponseType == REQUEST_CONTACT_DETAIL)
    {
        if(response.baseInfo.sexual == ProtocolType::MAN)
        {
            d->m_sex_edit->setText(QObject::tr("Man"));
        }
        else
        {
            d->m_sex_edit->setText(QObject::tr("Woman"));
        }
        d->m_birthDay_edit->setText(response.baseInfo.birthday);
        d->m_location_edit->setText(response.baseInfo.address);
    }
}

void ContactDetailWindow::remarkEditFinished()
{
    MQ_D(ContactDetailWindow);
    d->m_remark_edit->setReadOnly(true);
}
