#include "modifyremarkwindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "head.h"
#include "util/rsingleton.h"
#include "Util/imagemanager.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "widget/rbutton.h"

#define EDIT_REMARK_WIDTH 300
#define EDIT_REMARK_HEIGHT 25
#define WINDOW_WIDTH 330
#define WINDOW_HEIGHT 150

class ModifyRemarkWindowPrivate : public GlobalData<ModifyRemarkWindow>
{
    Q_DECLARE_PUBLIC(ModifyRemarkWindow)

private:
    ModifyRemarkWindowPrivate(ModifyRemarkWindow * q):q_ptr(q)
    {
        initWidget();
    }

    ModifyRemarkWindow * q_ptr;

    QWidget * contentWidget;

    QLabel * m_hint_label;
    QLineEdit * m_remark_edit;

    RButton * sureButton;
    RButton * cancelButton;

    void initWidget();
};

void ModifyRemarkWindowPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Widget_ContentWidget");

    const int Label_Width = 200;
    const int Label_Height = 20;

    QWidget * bodyWidget = new QWidget(contentWidget);

    m_hint_label = new QLabel(bodyWidget);
    m_hint_label->setAlignment(Qt::AlignLeft);
    m_hint_label->setFixedSize(Label_Width,Label_Height);
    m_hint_label->setText(QObject::tr("Please input remark name:"));

    m_remark_edit = new QLineEdit(bodyWidget);
    m_remark_edit->setFixedSize(EDIT_REMARK_WIDTH,EDIT_REMARK_HEIGHT);

    QVBoxLayout * bodyLayout = new QVBoxLayout;
    bodyLayout->setContentsMargins(10,20,10,20);
    bodyLayout->addWidget(m_hint_label);
    bodyLayout->addWidget(m_remark_edit);

    bodyWidget->setLayout(bodyLayout);

    QWidget * bottomWidget = new QWidget(contentWidget);

    sureButton = new RButton();
    sureButton->setText(QObject::tr("Sure"));

    QObject::connect(sureButton,SIGNAL(pressed()),q_ptr,SLOT(applyNewRemark()));

    cancelButton = new RButton();
    cancelButton->setText(QObject::tr("Cancel"));
    QObject::connect(cancelButton,SIGNAL(pressed()),q_ptr,SLOT(close()));

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,10,10);
    layout->addStretch(1);
    layout->addWidget(sureButton);
    layout->addWidget(cancelButton);

    bottomWidget->setLayout(layout);

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->addWidget(bodyWidget);
    contentLayout->addWidget(bottomWidget);
    contentWidget->setLayout(contentLayout);

    q_ptr->setContentWidget(contentWidget);
}

ModifyRemarkWindow::ModifyRemarkWindow(QWidget *parent):
    d_ptr(new ModifyRemarkWindowPrivate(this)),
    Widget(parent)
{
    setWindowFlags(windowFlags() & ~Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose,true);
    setWindowTitle(tr("Modify remark name"));
    setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::NORMAL)));

    RSingleton<Subject>::instance()->attach(this);

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowIcon(RSingleton<ImageManager>::instance()->getWindowIcon(ImageManager::WHITE,ImageManager::ICON_SYSTEM,ImageManager::ICON_16));

        bar->setWindowTitle(tr("Modify remark name"));
    }

    setFixedSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    QSize  screenSize = RUtil::screenSize();
    setGeometry((screenSize.width() - WINDOW_WIDTH)/2,(screenSize.height() - WINDOW_HEIGHT)/2,WINDOW_WIDTH,WINDOW_HEIGHT);

}

ModifyRemarkWindow::~ModifyRemarkWindow()
{
    RSingleton<Subject>::instance()->detach(this);
    delete d_ptr;
}

void ModifyRemarkWindow::onMessage(MessageType type)
{
    Q_UNUSED(type);
}

/*!
 * @brief 设置修改备注名窗口显示内容
 * @param[in] remarkName：QString，窗口中待显示的内容
 * @return 无
 */
void ModifyRemarkWindow::setOldRemarkName(QString remarkName)
{
    MQ_D(ModifyRemarkWindow);
    d->m_remark_edit->setText(remarkName);
    d->m_remark_edit->selectAll();
}

/*!
     * @brief 设置修改备注名窗口确定按钮响应槽
     * @param[in]
     * @return 无
     */
void ModifyRemarkWindow::applyNewRemark()
{
    MQ_D(ModifyRemarkWindow);
    emit setNewRemark(d->m_remark_edit->text());
    close();
}
