#include "rquickorderwidget.h"

#include <QStandardItemModel>
#include <QCloseEvent>
#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QListView>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QTextStream>
#include <QDir>
#include <QToolButton>

#include "head.h"
#include "protocol/datastruct.h"
#include "constants.h"
#include "global.h"
#include "user/user.h"
#include "util/rsingleton.h"

static RQuickOrderWidget * m_gRQuickOrderWidget=NULL;

class RQuickOrderWidgetPrivate : public GlobalData<RQuickOrderWidget>
{
    Q_DECLARE_PUBLIC(RQuickOrderWidget)

protected:
    explicit RQuickOrderWidgetPrivate(RQuickOrderWidget *q):
        q_ptr(q)
    {
        initWidget();
    }
    void initWidget();

    RQuickOrderWidget * q_ptr;
    QStandardItemModel *m_pModel;
    QListView *listView_Order;

    QWidget * contentWidget;
    ToolBar *windowToolBar;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QToolButton *pb_Add;
    QToolButton *pb_Delete;
    QSpacerItem *horizontalSpacer;
};

void RQuickOrderWidgetPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("ChatC2CWidget");
    verticalLayout = new QVBoxLayout(contentWidget);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    verticalLayout->setContentsMargins(0, 2, 0, 0);

    groupBox = new QGroupBox(contentWidget);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    verticalLayout_2 = new QVBoxLayout(groupBox);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    listView_Order = new QListView(groupBox);
    listView_Order->setObjectName(QString::fromUtf8("listView_Order"));
    verticalLayout_2->addWidget(listView_Order);
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    pb_Add = new QToolButton(groupBox);
    pb_Add->setIcon(QIcon(QString(":/icon/resource/icon/settings_add.png")));
    pb_Add->setObjectName(QString::fromUtf8("pb_Add"));
    horizontalLayout->addWidget(pb_Add);
    pb_Delete = new QToolButton(groupBox);
    pb_Delete->setIcon(QIcon(QString(":/icon/resource/icon/settings_delete.png")));
    pb_Delete->setObjectName(QString::fromUtf8("pb_Delete"));
    horizontalLayout->addWidget(pb_Delete);
    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout->addItem(horizontalSpacer);
    verticalLayout_2->addLayout(horizontalLayout);
    verticalLayout->addWidget(groupBox);

    m_pModel = new QStandardItemModel();
    listView_Order->setModel(m_pModel);
    QObject::connect(pb_Add,SIGNAL(clicked(bool)),q_ptr,SLOT(addItem(bool)));
    QObject::connect(pb_Delete,SIGNAL(clicked(bool)),q_ptr,SLOT(deleteItem(bool)));

    groupBox->setTitle(QObject::tr("RQuickOrderWidget"));
    pb_Add->setText(QObject::tr(" + "));
    pb_Delete->setText(QObject::tr(" - "));

    q_ptr->setContentWidget(contentWidget);
}

RQuickOrderWidget::RQuickOrderWidget(QWidget *parent) :
    d_ptr(new RQuickOrderWidgetPrivate(this)),
    Widget(parent)
{
    m_gRQuickOrderWidget = this;
    initOrderFilePath();
    readLocalOrderFile();

    ToolBar * bar = enableToolBar(true);
    enableDefaultSignalConection(true);
    if(bar)
    {
        bar->setToolFlags(ToolBar::TOOL_DIALOG);
        bar->setWindowTitle(tr("QuickOrderSetting"));
    }

    RSingleton<Subject>::instance()->attach(this);
}

RQuickOrderWidget::~RQuickOrderWidget()
{
    RSingleton<Subject>::instance()->detach(this);
}

RQuickOrderWidget *RQuickOrderWidget::instance()
{
    if(m_gRQuickOrderWidget==NULL)
    {
        m_gRQuickOrderWidget = new RQuickOrderWidget();
    }
    return m_gRQuickOrderWidget;
}

/*!
 * @brief 添加快捷命令
 */
void RQuickOrderWidget::addOrder(QString order)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = new QStandardItem(order);
    d->m_pModel->appendRow(item);
}

/*!
 * @brief 清空命令
 */
void RQuickOrderWidget::clearOrder()
{
    MQ_D(RQuickOrderWidget);

    d->m_pModel->clear();
}

/*!
 * @brief 获取当前快捷命令数量
 * @return 快捷命令数量
 */
int RQuickOrderWidget::orderCount()
{
    MQ_D(RQuickOrderWidget);

    return d->m_pModel->rowCount();
}

/*!
 * @brief 获取所有命令内容
 * @return 命令内容字符串组
 */
QStringList RQuickOrderWidget::getOrderList()
{
    MQ_D(RQuickOrderWidget);

    QStringList list;
    for(int i=0;i<orderCount();i++)
    {
        QStandardItem *item = d->m_pModel->item(i);
        list.append(item->text());
    }
    return list;
}

/*!
 * @brief 读取本地配置文件
 */
void RQuickOrderWidget::readLocalOrderFile()
{
    QFile file(m_filePath);
    if(!file.open(QFile::ReadOnly))
    {
        return ;
    }
    clearOrder();
    QTextStream stream(&file);
    stream.setCodec("utf8");
    while(!stream.atEnd())
    {
        QString order = stream.readLine();
        addOrder(order);
    }
    file.close();
}

void RQuickOrderWidget::writeOrderToLocalFile()
{
    QFile file(m_filePath);
    if(!file.open(QFile::WriteOnly))
    {
        return ;
    }

    QTextStream stream(&file);
    stream.setCodec("utf8");
    QStringList list = getOrderList();
    foreach(QString order,list)
    {
        stream<<order<<endl;
        stream.flush();
    }
    file.close();
}

/*!
 * @brief 设置默认的文件路径
 */
void RQuickOrderWidget::initOrderFilePath()
{
    m_userHome = QDir::currentPath() + QDir::separator() + Constant::PATH_UserPath + QDir::separator() + G_User->BaseInfo().accountId;
    QString path = m_userHome + QDir::separator() + Constant::QuickOrder_ConfigName;
    setOrderFilePath(path);
    creatDefaultOrderFile();
}

void RQuickOrderWidget::setOrderFilePath(QString path)
{
    m_filePath = path;
}

void RQuickOrderWidget::onMessage(MessageType type)
{

}

/*!
 * @brief 界面关闭事件
 * @param event
 */
void RQuickOrderWidget::closeEvent(QCloseEvent *event)
{
    if(event->type() == QEvent::Close)
    {
        writeOrderToLocalFile();
        emit setComplete(getOrderList());
    }
}

/*!
 * @brief 添加一条新的命令条目
 */
void RQuickOrderWidget::addItem(bool)
{
    addOrder(QString::number(orderCount()));
}

/*!
 * @brief 删除选中的命令条目
 */
void RQuickOrderWidget::deleteItem(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pModel->itemFromIndex(d->listView_Order->currentIndex());
    if(item)
    {
        d->m_pModel->removeRow(d->listView_Order->currentIndex().row());
    }
}

/*!
 * @brief 创建一个默认的命令配置文件
 */
void RQuickOrderWidget::creatDefaultOrderFile()
{
    QFileInfo fileInfo(m_filePath);
    if(!fileInfo.exists())
    {
        addOrder(tr("OK"));
        addOrder(tr("ERROR"));
        addOrder(tr("Check"));
        addOrder(tr("CommeBack"));
        addOrder(tr("GoOn"));
        writeOrderToLocalFile();
    }
}
