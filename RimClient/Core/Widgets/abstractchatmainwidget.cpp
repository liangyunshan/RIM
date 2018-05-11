#include "abstractchatmainwidget.h"

#include <QWebEngineView>
#include <QWebEnginePage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWebChannel>
#include <QDateTime>
#include <QSplitter>

#include "head.h"
#include "global.h"
#include "datastruct.h"
#include "constants.h"
#include "toolbar.h"
#include "Util/rutil.h"
#include "widget/rlabel.h"
#include "widget/rbutton.h"
#include "rsingleton.h"
#include "previewpage.h"
#include "document.h"
#include "setfontwidget.h"

class AbstractChatMainWidgetPrivate : public GlobalData<AbstractChatMainWidget>
{
    Q_DECLARE_PUBLIC(AbstractChatMainWidget)

protected:
    AbstractChatMainWidgetPrivate(AbstractChatMainWidget *q):
        q_ptr(q)
    {
        m_content.setUi(q);
        initWidget();
    }

    void initWidget();

    AbstractChatMainWidget * q_ptr;

    QWidget * chatRecordWidget;             //聊天信息记录窗口
    QWidget * inputWidget;                  //聊天信息输入窗口
    QWidget * rightSideWidget;              //右侧边栏窗口

    QWebEngineView * view;                  //嵌入html页视图
    PreviewPage *page;                      //嵌入html页page
    Document m_content;                     //html与Qt交互跳板

    SetFontWidget *fontWidget;

};

void AbstractChatMainWidgetPrivate::initWidget()
{
    //主窗口水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(6,0,0,0);
    mainLayout->setSpacing(5);

    //左窗口垂直布局
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(0);

    //聊天信息记录窗口
    chatRecordWidget = new QWidget(q_ptr);
    page = new PreviewPage(chatRecordWidget);
    view = new QWebEngineView(chatRecordWidget);
    view->setPage(page);
    QObject::connect(view,SIGNAL(loadFinished(bool)),q_ptr,SLOT(finishLoadHTML(bool)));

    //HTML与Qt交互channel
    QWebChannel *channel = new QWebChannel(q_ptr);
    channel->registerObject(QStringLiteral("content"),&m_content);
    page->setWebChannel(channel);

    //信息输入窗口
    inputWidget = new QWidget(q_ptr);
    QVBoxLayout *inputLayout = new QVBoxLayout;
    inputLayout->setContentsMargins(0,0,0,0);
    inputLayout->setSpacing(0);
    inputWidget->setLayout(inputLayout);

    fontWidget = new SetFontWidget(inputWidget);
    //TODO LYS-20180511 连接字体选择窗口中信号到目标槽函数

    inputLayout->addWidget(fontWidget);

    QSplitter *chatSplitter = new QSplitter(Qt::Vertical);
    chatSplitter->addWidget(chatRecordWidget);
    chatSplitter->addWidget(inputWidget);
    chatSplitter->setStretchFactor(0,5);
    chatSplitter->setStretchFactor(1,1);

    leftLayout->addWidget(chatSplitter);

    rightSideWidget = QWidget(q_ptr);
    //TODO LYS-20180510 拓展为可添加多个tab窗口

    mainLayout->addLayout(leftLayout);
    mainLayout->addWidget(rightSideWidget);
    q_ptr->setLayout(mainLayout);

}


AbstractChatMainWidget::AbstractChatMainWidget(QWidget *parent) : QWidget(parent)
{

}

/*!
 * \brief AbstractChatMainWidget::inserHtml 嵌入html文件
 * \param htmlUrl 待嵌入的html地址
 */
void AbstractChatMainWidget::inserHtml(QUrl &htmlUrl)
{

}

/*!
 * \brief AbstractChatMainWidget::finishLoadHTML 加载完成html后对html进行初始化处理
 */
void AbstractChatMainWidget::finishLoadHTML(bool)
{
    //TODO LYS-20180510 初始化html中必要信息
}
