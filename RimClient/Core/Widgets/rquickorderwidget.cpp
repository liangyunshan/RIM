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
#include <QLabel>
#include <QSettings>
#include <QList>
#include <QMessageBox>

#include "head.h"
#include "protocol/datastruct.h"
#include "constants.h"
#include "global.h"
#include "user/user.h"
#include "util/rsingleton.h"
#include "widget/rbutton.h"

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
    QStandardItemModel *m_pGroupsModel;
    QStandardItemModel *m_pOpenGroupModel;
    QStandardItemModel *m_pAllOrderModel;
    QListView *listView_Groups;
    QListView *listView_OpenGroup;
    QListView *listView_AllOrder;

    QWidget * contentWidget;
    RButton *pb_Add_Group;
    RButton *pb_Delete_Group;
    RButton *pb_Clone_Group;
    RButton *pb_Add_ToAllOrder;
    RButton *pb_Delete_FromAllOrder;
    RButton *pb_Move_toCurr;
    RButton *pb_delete_FromCurr;
    RButton *pb_MoveUp;
    RButton *pb_MoveDown;
    RButton *pb_OK;
    RButton *pb_Cancel;

    QLabel *currOpenGroupName;
};

void RQuickOrderWidgetPrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("RQuickOrderWidget");
    contentWidget->setMinimumSize(600,400);

    QVBoxLayout *verticalLayout_6 = new QVBoxLayout(contentWidget);
    verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
    verticalLayout_6->setContentsMargins(10,2,10,2);

    QHBoxLayout *horizontalLayout_up = new QHBoxLayout();
    horizontalLayout_up->setObjectName(QString::fromUtf8("horizontalLayout_up"));
    horizontalLayout_up->setContentsMargins(2,5,2,5);

    //命令组列表视图
    QWidget *widget = new QWidget(contentWidget);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setMaximumWidth(220);
    QVBoxLayout *verticalLayout_3 = new QVBoxLayout(widget);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    verticalLayout_3->setContentsMargins(2,2,2,2);
    QLabel *label_3 = new QLabel(widget);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setText(QObject::tr("Group template"));
    verticalLayout_3->addWidget(label_3);
    QVBoxLayout *verticalLayout = new QVBoxLayout();
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    listView_Groups = new QListView(widget);
    listView_Groups->setObjectName(QString::fromUtf8("listView_Groups"));
    verticalLayout->addWidget(listView_Groups);
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setContentsMargins(5,2,5,2);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    pb_Clone_Group = new RButton(widget);
    pb_Clone_Group->setObjectName(QString::fromUtf8("pb_Clone_Group"));
    pb_Clone_Group->setText(QObject::tr("Clone"));
    pb_Clone_Group->setToolTip(QObject::tr("Clone group"));
    horizontalLayout->addWidget(pb_Clone_Group);
    pb_Add_Group = new RButton(widget);
    pb_Add_Group->setObjectName(QString::fromUtf8("pb_Add_Group"));
    pb_Add_Group->setText(QObject::tr("Add new group"));
    pb_Add_Group->setToolTip(QObject::tr("Add new group"));
    horizontalLayout->addWidget(pb_Add_Group);
    pb_Delete_Group = new RButton(widget);
    pb_Delete_Group->setObjectName(QString::fromUtf8("pb_Delete_Group"));
    pb_Delete_Group->setText(QObject::tr("Delete group"));
    pb_Delete_Group->setToolTip(QObject::tr("Delete group"));
    horizontalLayout->addWidget(pb_Delete_Group);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout_3->addLayout(verticalLayout);
    horizontalLayout_up->addWidget(widget);

    //打开的当前命令组列表视图
    QWidget *widget_OpenGroup = new QWidget(contentWidget);
    widget_OpenGroup->setMaximumWidth(220);
    widget_OpenGroup->setObjectName(QString::fromUtf8("widget_OpenGroup"));
    QVBoxLayout *verticalLayout_OpenGroup = new QVBoxLayout(widget_OpenGroup);
    verticalLayout_OpenGroup->setObjectName(QString::fromUtf8("verticalLayout_OpenGroup"));
    verticalLayout_OpenGroup->setContentsMargins(2,2,2,2);
    currOpenGroupName = new QLabel(widget_OpenGroup);
    currOpenGroupName->setObjectName(QString::fromUtf8("currOpenGroupName"));
    currOpenGroupName->setText(QObject::tr("group orders"));
    verticalLayout_OpenGroup->addWidget(currOpenGroupName);
    listView_OpenGroup = new QListView(widget_OpenGroup);
    listView_OpenGroup->setObjectName(QString::fromUtf8("listView_OpenGroup"));
    verticalLayout_OpenGroup->addWidget(listView_OpenGroup);
    horizontalLayout_up->addWidget(widget_OpenGroup);

    //移动按钮区域
    QFrame *frame = new QFrame(contentWidget);
    frame->setObjectName(QString::fromUtf8("frame"));
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);
    QVBoxLayout *verticalLayout_2 = new QVBoxLayout(frame);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    pb_Move_toCurr = new RButton(frame);
    pb_Move_toCurr->setObjectName(QString::fromUtf8("pb_Move_toCurr"));
    pb_Move_toCurr->setText(QObject::tr("<-"));
    pb_Move_toCurr->setToolTip(QObject::tr("move order to group"));
    verticalLayout_2->addWidget(pb_Move_toCurr);
    pb_delete_FromCurr = new RButton(frame);
    pb_delete_FromCurr->setObjectName(QString::fromUtf8("pb_delete_FromCurr"));
    pb_delete_FromCurr->setText(QObject::tr("X"));
    pb_delete_FromCurr->setToolTip(QObject::tr("delete from group"));
    verticalLayout_2->addWidget(pb_delete_FromCurr);
    pb_MoveUp = new RButton(frame);
    pb_MoveUp->setObjectName(QString::fromUtf8("pb_MoveUp"));
    pb_MoveUp->setText(QObject::tr("MoveUp"));
    pb_MoveUp->setToolTip(QObject::tr("MoveUp"));
    verticalLayout_2->addWidget(pb_MoveUp);
    pb_MoveDown = new RButton(frame);
    pb_MoveDown->setObjectName(QString::fromUtf8("pb_MoveDown"));
    pb_MoveDown->setText(QObject::tr("MoveDown"));
    pb_MoveDown->setToolTip(QObject::tr("MoveDown"));
    verticalLayout_2->addWidget(pb_MoveDown);
    horizontalLayout_up->addWidget(frame);

    //全部命令区域
    QWidget *widget_AllOrder = new QWidget(contentWidget);
    widget_AllOrder->setMaximumWidth(220);
    widget_AllOrder->setObjectName(QString::fromUtf8("widget_AllOrder"));
    QVBoxLayout *verticalLayout_AllOrder = new QVBoxLayout(widget_AllOrder);
    verticalLayout_AllOrder->setObjectName(QString::fromUtf8("verticalLayout_AllOrder"));
    verticalLayout_AllOrder->setContentsMargins(2,2,2,2);
    QLabel *label_AllOrder = new QLabel(widget_AllOrder);
    label_AllOrder->setObjectName(QString::fromUtf8("label_AllOrder"));
    label_AllOrder->setText(QObject::tr("AllOrders"));
    verticalLayout_AllOrder->addWidget(label_AllOrder);
    listView_AllOrder = new QListView(widget_AllOrder);
//    listView_AllOrder->setMaximumWidth(80);
    listView_AllOrder->setObjectName(QString::fromUtf8("listView_AllOrder"));
    verticalLayout_AllOrder->addWidget(listView_AllOrder);
    QHBoxLayout *horizontalLayout_AllOrder = new QHBoxLayout();
    horizontalLayout_AllOrder->setObjectName(QString::fromUtf8("horizontalLayout_AllOrder"));
    pb_Add_ToAllOrder = new RButton(widget);
    pb_Add_ToAllOrder->setObjectName(QString::fromUtf8("pb_Add_ToAllOrder"));
    pb_Add_ToAllOrder->setText(QObject::tr("Add Order"));
    pb_Add_ToAllOrder->setToolTip(QObject::tr("Add New Order"));
    horizontalLayout_AllOrder->addWidget(pb_Add_ToAllOrder);
    pb_Delete_FromAllOrder = new RButton(widget);
    pb_Delete_FromAllOrder->setObjectName(QString::fromUtf8("pb_Delete_FromAllOrder"));
    pb_Delete_FromAllOrder->setText(QObject::tr("Delete Order"));
    pb_Delete_FromAllOrder->setToolTip(QObject::tr("Delete Order"));
    horizontalLayout_AllOrder->addWidget(pb_Delete_FromAllOrder);
    QSpacerItem *horizontalSpacer_AllOrder = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_AllOrder->addItem(horizontalSpacer_AllOrder);
    verticalLayout_AllOrder->addLayout(horizontalLayout_AllOrder);
    horizontalLayout_up->addWidget(widget_AllOrder);
    verticalLayout_6->addLayout(horizontalLayout_up);

    //界面底部水平分隔线
    QFrame *line = new QFrame(contentWidget);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    verticalLayout_6->addWidget(line);

    //界面底部确认取消按钮
    QHBoxLayout *horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
    horizontalLayout_2->setContentsMargins(0,0,5,5);
    horizontalLayout_2->setSpacing(5);
    QSpacerItem *horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    horizontalLayout_2->addItem(horizontalSpacer_2);
    pb_OK = new RButton(contentWidget);
    pb_OK->setObjectName(QString::fromUtf8("pb_OK"));
    pb_OK->setText(QObject::tr("OK"));
    pb_OK->setToolTip(QObject::tr("OK"));
    horizontalLayout_2->addWidget(pb_OK);
    pb_Cancel = new RButton(contentWidget);
    pb_Cancel->setObjectName(QString::fromUtf8("pb_Cancel"));
    pb_Cancel->setText(QObject::tr("Cancel"));
    pb_Cancel->setToolTip(QObject::tr("Cancel"));
    horizontalLayout_2->addWidget(pb_Cancel);
    verticalLayout_6->addLayout(horizontalLayout_2);

    q_ptr->setContentWidget(contentWidget);

    //初始化列表模型
    m_pGroupsModel = new QStandardItemModel();
    listView_Groups->setModel(m_pGroupsModel);
    m_pOpenGroupModel = new QStandardItemModel();
    listView_OpenGroup->setModel(m_pOpenGroupModel);
    listView_OpenGroup->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_pAllOrderModel = new QStandardItemModel();
    listView_AllOrder->setModel(m_pAllOrderModel);

    //初始化界面信号槽
    QObject::connect(pb_Add_Group,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickAddNewGroup(bool)));
    QObject::connect(pb_Delete_Group,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickDeleteGroup(bool)));
    QObject::connect(pb_Clone_Group,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickCloneGroup(bool)));

    QObject::connect(pb_Move_toCurr,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickMoveToCurrGroup(bool)));
    QObject::connect(pb_delete_FromCurr,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickDeleteFromCurrGroup(bool)));
    QObject::connect(pb_MoveUp,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickMoveUpInCurrGroup(bool)));
    QObject::connect(pb_MoveDown,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickMoveDownInCurrGroup(bool)));

    QObject::connect(pb_Add_ToAllOrder,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickAddNewOrder(bool)));
    QObject::connect(pb_Delete_FromAllOrder,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickDeleteOrder(bool)));

    QObject::connect(pb_OK,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickOKButton(bool)));
    QObject::connect(pb_Cancel,SIGNAL(clicked(bool)),
            q_ptr,SLOT(clickCancelButton(bool)));

    QObject::connect(listView_Groups,SIGNAL(clicked(QModelIndex)),
            q_ptr,SLOT(groupItemClick(QModelIndex)));
    QObject::connect(m_pAllOrderModel,SIGNAL(itemChanged(QStandardItem*)),
            q_ptr,SLOT(allOrderItemChanged(QStandardItem*)));
    QObject::connect(m_pGroupsModel,SIGNAL(itemChanged(QStandardItem*)),
            q_ptr,SLOT(groupItemChanged(QStandardItem*)));
}

RQuickOrderWidget::RQuickOrderWidget(QWidget *parent) :
    d_ptr(new RQuickOrderWidgetPrivate(this)),
    Widget(parent)
{
    m_gRQuickOrderWidget = this;
    m_pUserSettings = NULL;
    groupIsChanged = false;
    allOrderIsChanged = false;
    initOrderFilePath();

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

void RQuickOrderWidget::onMessage(MessageType type)
{

}

/*!
 * @brief 添加快捷命令
 */
void RQuickOrderWidget::addOrder(QString order)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = new QStandardItem(order);
    item->setToolTip(order);
    d->m_pAllOrderModel->appendRow(item);

    m_allOrderList_Temp.orderlist.append(order);
}

/*!
 * @brief 获取当前快捷命令数量
 * @return 快捷命令数量
 */
int RQuickOrderWidget::AllOrderCount()
{
    MQ_D(RQuickOrderWidget);

    return d->m_pAllOrderModel->rowCount();
}

QStringList RQuickOrderWidget::getCurrOrderList()
{
    foreach(OrderGroup *group,m_groupList.groupList)
    {
        if(m_groupList.currGroupName == group->groupName)
        {
            return group->orderlist;
        }
    }
    return QStringList();
}

/*!
 * @brief 获取所有命令内容
 * @return 命令内容字符串组
 */
QStringList RQuickOrderWidget::getOrderList()
{
    MQ_D(RQuickOrderWidget);

    QStringList list;
    for(int i=0;i<AllOrderCount();i++)
    {
        QStandardItem *item = d->m_pAllOrderModel->item(i);
        list.append(item->text());
    }
    return list;
}

QStringList RQuickOrderWidget::getOpenGroupOrderList()
{
    MQ_D(RQuickOrderWidget);

    QStringList list;
    for(int i=0;i<d->m_pOpenGroupModel->rowCount();i++)
    {
        QStandardItem *item = d->m_pOpenGroupModel->item(i);
        list.append(item->text());
    }
    return list;
}

QStringList RQuickOrderWidget::getGroupsNameList()
{
    MQ_D(RQuickOrderWidget);

    QStringList list;
    for(int i=0;i<d->m_pGroupsModel->rowCount();i++)
    {
        QStandardItem *item = d->m_pGroupsModel->item(i);
        list.append(item->text());
    }
    return list;
}

/*!
 * @brief 获取当前设置的所有组和对应的命令
 * @return 所有组和每组包含的命令
 */
QStringList RQuickOrderWidget::getAllGroupsOrderList()
{
    QStringList list ;
    foreach(OrderGroup *group,m_groupList.groupList)
    {
        list.append(group->groupName);
        list.append(group->orderlist);
    }
    return list;
}

/*!
 * @brief 界面关闭事件
 * @param event
 */
void RQuickOrderWidget::closeEvent(QCloseEvent *event)
{
    if(event->type() == QEvent::Close)
    {
        clickCancelButton(true);
    }
}

/*!
 * @brief 确认按钮，退出编辑界面
 */
void RQuickOrderWidget::clickOKButton(bool)
{
    syncTempToReal();
    updateAllOrderToFile();
    updateGroupsToFile();

    this->close();
}

/*!
 * @brief 取消按钮，退出编辑界面
 */
void RQuickOrderWidget::clickCancelButton(bool)
{
    if(m_groupList != m_groupList_Temp || m_allOrderList != m_allOrderList_Temp)
    {
        QMessageBox::StandardButton button = QMessageBox::information(this,tr("information"),
                                 tr("Ui data has changed,sure change?"),
                                 QMessageBox::Ok|QMessageBox::No);
        if(button == QMessageBox::Ok)
        {
            syncTempToReal();
            updateAllOrderToFile();
            updateGroupsToFile();
        }
        else
        {
            rollbackTempFromReal();
            updateAllOrderListView();
            resetGroupsListView();
        }
    }
    this->close();
}

void RQuickOrderWidget::clickAddNewGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    addNewGroup(tr("NewGroup_%1").arg(d->m_pGroupsModel->rowCount()),QStringList());
}

void RQuickOrderWidget::clickDeleteGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pGroupsModel->itemFromIndex(d->listView_Groups->currentIndex());
    if(item)
    {
        foreach(OrderGroup *group,m_groupList_Temp.groupList)
        {
            if(group->groupName == item->text() )
            {
                delete group;
                m_groupList_Temp.groupList.removeOne(group);
                break;
            }
        }
        d->m_pGroupsModel->removeRow(d->listView_Groups->currentIndex().row());
    }

    QStandardItem *curritem = d->m_pGroupsModel->itemFromIndex(d->listView_Groups->currentIndex());
    if(curritem)
    {
        setCurrGroup(curritem->text());
    }
    else
    {
        setCurrGroup(QString(""));
    }
}

void RQuickOrderWidget::clickCloneGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pGroupsModel->itemFromIndex(d->listView_Groups->currentIndex());
    if(item)
    {
        foreach(OrderGroup *group,m_groupList_Temp.groupList)
        {
            if(group->groupName == m_groupList_Temp.currGroupName)
            {
                addNewGroup(QString("%1_%2").arg(item->text()).arg(tr("Clone")),
                            group->orderlist);
                break;
            }
        }
    }
}

void RQuickOrderWidget::clickMoveToCurrGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pAllOrderModel->itemFromIndex(d->listView_AllOrder->currentIndex());
    if(item)
    {
        if(!orderIsExitedInOpenGroup(item->text()))
        {
            if(m_groupList_Temp.groupList.count()==0)
            {
                QStringList list;
                list.append(item->text());
                addNewGroup(tr("default"),list);
            }
            else
            {
                QStandardItem *newitem = new QStandardItem(item->text());
                newitem->setToolTip(item->text());
                d->m_pOpenGroupModel->appendRow(newitem);
                d->listView_OpenGroup->setCurrentIndex(newitem->index());
                updateOpenGroup();
            }
        }
    }
}

void RQuickOrderWidget::clickDeleteFromCurrGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pOpenGroupModel->itemFromIndex(d->listView_OpenGroup->currentIndex());
    if(item)
    {
        d->m_pOpenGroupModel->removeRow(d->listView_OpenGroup->currentIndex().row());
        updateOpenGroup();
    }
}

void RQuickOrderWidget::clickMoveUpInCurrGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pOpenGroupModel->itemFromIndex(d->listView_OpenGroup->currentIndex());
    if(item)
    {
        int currRow = d->listView_OpenGroup->currentIndex().row();
        if(currRow>0)
        {
            QStandardItem *newitem = new QStandardItem(item->text());
            d->m_pOpenGroupModel->removeRow(currRow);
            d->m_pOpenGroupModel->insertRow(currRow-1,newitem);
            d->listView_OpenGroup->setCurrentIndex(newitem->index());

            updateOpenGroup();
        }
    }
}

void RQuickOrderWidget::clickMoveDownInCurrGroup(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pOpenGroupModel->itemFromIndex(d->listView_OpenGroup->currentIndex());
    if(item)
    {
        int currRow = d->listView_OpenGroup->currentIndex().row();
        if(currRow < d->m_pOpenGroupModel->rowCount()-1)
        {
            QStandardItem *newitem = new QStandardItem(item->text());
            d->m_pOpenGroupModel->removeRow(currRow);
            d->m_pOpenGroupModel->insertRow(currRow+1,newitem);
            d->listView_OpenGroup->setCurrentIndex(newitem->index());

            updateOpenGroup();
        }
    }
}

void RQuickOrderWidget::updateOpenGroup()
{
    MQ_D(RQuickOrderWidget);

    OrderGroup *currGroup = NULL;
    foreach(currGroup,m_groupList_Temp.groupList)
    {
        if(currGroup->groupName == m_groupList_Temp.currGroupName)
        {
            currGroup->orderlist.clear();
            break;
        }
    }

    for(int index=0;index<d->m_pOpenGroupModel->rowCount();index++)
    {
        QStandardItem *item = d->m_pOpenGroupModel->item(index);
        if(item && currGroup)
        {
            currGroup->orderlist.append(item->text());
        }
    }
}

/*!
 * @brief 添加一条新的命令条目
 */
void RQuickOrderWidget::clickAddNewOrder(bool)
{
    addOrder(QString::number(AllOrderCount()));
}

/*!
 * @brief 删除选中的命令条目
 */
void RQuickOrderWidget::clickDeleteOrder(bool)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pAllOrderModel->itemFromIndex(d->listView_AllOrder->currentIndex());
    if(item)
    {
        QString deleteOrder = item->text();
        m_allOrderList_Temp.orderlist.removeOne(deleteOrder);
        d->m_pAllOrderModel->removeRow(d->listView_AllOrder->currentIndex().row());

        int orderIndex = -1;
        foreach(OrderGroup *group,m_groupList_Temp.groupList)
        {
            orderIndex = group->orderlist.indexOf(deleteOrder);
            if(orderIndex>=0)
            {
                group->orderlist.removeOne(deleteOrder);
            }
            orderIndex = -1;
        }

        updateCurrOrderListView();
    }
}

void RQuickOrderWidget::groupItemClick(const QModelIndex &index)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = d->m_pGroupsModel->itemFromIndex(index);
    if(item)
    {
        setCurrGroup(item->text());
    }
}

/*!
 * @brief 组别列表单元内容发生变化
 * @param item 产生变化的单元条目
 */
void RQuickOrderWidget::groupItemChanged(QStandardItem *item)
{
    if(item)
    {
        QString preOrder = item->toolTip();
        if(preOrder == item->text())
        {
            return ;
        }
        foreach(OrderGroup *group,m_groupList_Temp.groupList)
        {
            if(group->groupName == preOrder)
            {
                group->groupName = item->text();
                m_groupList_Temp.currGroupName = item->text();
                break;
            }
        }
        item->setToolTip(item->text());
        setCurrGroup(m_groupList_Temp.currGroupName);
    }
}

/*!
 * @brief 所有命令显示列表中条目内容产生变化的槽函数
 * @param item 产生变化的单元条目
 */
void RQuickOrderWidget::allOrderItemChanged(QStandardItem *item)
{
    if(item)
    {
        QString preOrder = item->toolTip();
        if(preOrder == item->text())
        {
            return ;
        }
        int index = m_allOrderList_Temp.orderlist.indexOf(preOrder);
        if(index>=0)
        {
            m_allOrderList_Temp.orderlist.replace(index,item->text());

            int orderIndex = -1;
            foreach(OrderGroup *group,m_groupList_Temp.groupList)
            {
                orderIndex = group->orderlist.indexOf(preOrder);
                if(orderIndex>=0)
                {
                    group->orderlist.replace(orderIndex,item->text());
                }
                orderIndex = -1;
            }
            item->setToolTip(item->text());

            updateCurrOrderListView();
        }
    }
}

void RQuickOrderWidget::addNewGroup(QString groupName, QStringList orders)
{
    MQ_D(RQuickOrderWidget);

    QStandardItem *item = new QStandardItem(groupName);
    item->setToolTip(groupName);
    d->m_pGroupsModel->appendRow(item);
    d->listView_Groups->setCurrentIndex(item->index());

    OrderGroup *group = new OrderGroup;
    group->groupName = groupName;
    group->orderlist = orders;
    m_groupList_Temp.currGroupName = groupName;
    m_groupList_Temp.groupList.append(group);

    updateCurrOrderListView();
}

void RQuickOrderWidget::addOrderToOpenGroup(QStringList orders)
{
    MQ_D(RQuickOrderWidget);

    foreach(QString order,orders)
    {
        QStandardItem *item = new QStandardItem(order);
        item->setToolTip(order);
        d->m_pOpenGroupModel->appendRow(item);
    }
}

/*!
 * @brief 重置刷新显示的组别列表
 */
void RQuickOrderWidget::resetGroupsListView()
{
    MQ_D(RQuickOrderWidget);

    d->m_pGroupsModel->clear();
    m_groupList_Temp.groupList.clear();

    foreach(OrderGroup *group,m_groupList.groupList)
    {
        addNewGroup(group->groupName,group->orderlist);
    }

    setCurrGroup(m_groupList.currGroupName);
}

void RQuickOrderWidget::setCurrGroup(QString groupName)
{
    MQ_D(RQuickOrderWidget);

    if(groupName.isEmpty())
    {
        m_groupList_Temp.currGroupName = groupName;
        updateCurrOrderListView();
    }
    for(int row=0;row<d->m_pGroupsModel->rowCount();row++)
    {
        QStandardItem *item = d->m_pGroupsModel->item(row);
        if(item)
        {
            if(item->text() == groupName)
            {
                d->listView_Groups->setCurrentIndex(item->index());
                m_groupList_Temp.currGroupName = groupName;

                updateCurrOrderListView();
                break;
            }
        }
    }
}

/*!
 * @brief 刷新显示的当前模板命令
 */
void RQuickOrderWidget::updateCurrOrderListView()
{
    MQ_D(RQuickOrderWidget);

    d->m_pOpenGroupModel->clear();
    d->currOpenGroupName->setText(tr("CurrGroup: %1").arg(m_groupList_Temp.currGroupName));

    foreach(OrderGroup *group,m_groupList_Temp.groupList)
    {
        if(group->groupName == m_groupList_Temp.currGroupName)
        {
            addOrderToOpenGroup(group->orderlist);
            break;
        }
    }
}

/*!
 * @brief 刷新显示的当前模板命令
 */
void RQuickOrderWidget::updateCurrOrderListView(QStringList orders)
{
    MQ_D(RQuickOrderWidget);

    d->m_pOpenGroupModel->clear();
    addOrderToOpenGroup(orders);
}

/*!
 * @brief 刷新显示所有命令列表
 */
void RQuickOrderWidget::updateAllOrderListView()
{
    MQ_D(RQuickOrderWidget);

    d->m_pAllOrderModel->clear();
    m_allOrderList_Temp.orderlist.clear();

    foreach(QString order,m_allOrderList.orderlist)
    {
        addOrder(order);
    }
}

/*!
 * @brief 创建一个默认的命令配置文件
 */
void RQuickOrderWidget::creatDefaultOrderFile()
{
    createDefaultAllOrder();
    createDefaultGroup();
    syncTempToReal();
    updateAllOrderToFile();
    updateGroupsToFile();
}

/*!
 * @brief 清空命令
 */
void RQuickOrderWidget::clearOrderFromAllOrder()
{
    MQ_D(RQuickOrderWidget);

    d->m_pAllOrderModel->clear();
}

/*!
 * @brief 初始化命令文件
 */
void RQuickOrderWidget::initOrderFilePath()
{
    m_userHome = QDir::currentPath() + QDir::separator() + Constant::PATH_UserPath + QDir::separator() + G_User->BaseInfo().accountId;
    QString path = m_userHome + QDir::separator() + Constant::QuickOrder_ConfigName;
    setOrderFilePath(path);

    QFileInfo fileInfo(m_filePath);
    if(!fileInfo.exists() || fileInfo.size() <= 5)
    {
        creatDefaultOrderFile();
    }
    else
    {
        readSettings();
    }
}

/*!
 * @brief 设置默认的文件路径
 */
void RQuickOrderWidget::setOrderFilePath(QString path)
{
    m_pUserSettings = new QSettings(path,QSettings::IniFormat);
    m_filePath = path;
}

bool RQuickOrderWidget::orderIsExitedInOpenGroup(QString orderin)
{
    QStringList list = getOpenGroupOrderList();
    foreach(QString order,list)
    {
        if(orderin == order)
        {
            return true;
        }
    }
    return false;
}

/*!
 * @brief 创建默认的命令
 */
void RQuickOrderWidget::createDefaultAllOrder()
{
    addOrder(tr("Check"));
    addOrder(tr("ComeBack"));
    addOrder(tr("Go On"));
    addOrder(tr("Stay"));
}

/*!
 * @brief 创建默认的命令组
 */
void RQuickOrderWidget::createDefaultGroup()
{
    addNewGroup(tr("DefaultGroup"),getOrderList());
}

void RQuickOrderWidget::readSettings()
{
    if(m_pUserSettings==NULL)
    {
        return ;
    }

    readAllOrderFromFile();
    readGroupsFromFile();

    rollbackTempFromReal();
    resetGroupsListView();
    updateAllOrderListView();
}

void RQuickOrderWidget::updateAllOrderToFile()
{
    if(m_pUserSettings==NULL)
    {
        return ;
    }
    QSettings * settings = m_pUserSettings;
    settings->remove(QUICKORDER_SETTINGS_ORDER);

    settings->beginGroup(QUICKORDER_SETTINGS_ORDER);
    settings->setValue(QUICKORDER_SETTINGS_ORDER_COUNT,m_allOrderList.orderlist.count());
    int orderNum=0;
    foreach(QString order,m_allOrderList.orderlist)
    {
        orderNum++;
        settings->setValue(QString("%1%2").arg(QUICKORDER_SETTINGS_ORDER_UNIT).arg(orderNum),order);
    }
    settings->endGroup();
}

void RQuickOrderWidget::readAllOrderFromFile()
{
    if(m_pUserSettings==NULL)
    {
        return ;
    }
    QSettings * settings = m_pUserSettings;

    settings->beginGroup(QUICKORDER_SETTINGS_ORDER);
    int allCount = settings->value(QUICKORDER_SETTINGS_ORDER_COUNT).toInt();
    for(int orderIndex=1;orderIndex<=allCount;orderIndex++)
    {
        QString order = settings->value(QString("%1%2").arg(QUICKORDER_SETTINGS_ORDER_UNIT).arg(orderIndex)).toString();
        m_allOrderList.orderlist.append(order);
    }
    settings->endGroup();
}

void RQuickOrderWidget::updateGroupsToFile()
{
    if(m_pUserSettings==NULL)
    {
        return ;
    }
    QSettings * settings = m_pUserSettings;
    clearGroupsToFile();

    settings->beginGroup(QUICKORDER_SETTINGS_GROUP);
    settings->setValue(QUICKORDER_SETTINGS_GROUP_TOTLECOUNT,m_groupList.groupList.count());
    settings->setValue(QUICKORDER_SETTINGS_GROUP_CURR,m_groupList.currGroupName);
    settings->endGroup();

    int num = 0;
    foreach (OrderGroup *group, m_groupList.groupList)
    {
        num++;
        QString section = QString("%1_%2").arg(QUICKORDER_SETTINGS_GROUP).arg(num);
        settings->beginGroup(section);
        settings->setValue(QUICKORDER_SETTINGS_GROUP_NAME,group->groupName);
        settings->setValue(QUICKORDER_SETTINGS_GROUP_ORDERCOUNT,group->orderlist.count());
        int index = 0;
        QString orderKey;
        foreach(QString orderValue,group->orderlist)
        {
            index++;
            orderKey = QString("%1_%2").arg(QUICKORDER_SETTINGS_GROUP_ORDER).arg(index);
            settings->setValue(orderKey,orderValue);
        }
        settings->endGroup();
    }
}

void RQuickOrderWidget::readGroupsFromFile()
{
    if(m_pUserSettings==NULL)
    {
        return ;
    }
    QSettings * settings = m_pUserSettings;

    settings->beginGroup(QUICKORDER_SETTINGS_GROUP);
    int groupCount = settings->value(QUICKORDER_SETTINGS_GROUP_TOTLECOUNT).toInt();
    QString currGroupName = settings->value(QUICKORDER_SETTINGS_GROUP_CURR).toString();
    m_groupList.currGroupName = currGroupName;
    settings->endGroup();

    for(int groupIndex=1;groupIndex<=groupCount;groupIndex++)
    {
        OrderGroup *group = new OrderGroup;
        QString section = QString("%1_%2").arg(QUICKORDER_SETTINGS_GROUP).arg(groupIndex);
        settings->beginGroup(section);
        group->groupName = settings->value(QUICKORDER_SETTINGS_GROUP_NAME).toString();
        int orderCount = settings->value(QUICKORDER_SETTINGS_GROUP_ORDERCOUNT).toInt();

        QString orderKey;
        for(int index=1;index<=orderCount;index++)
        {
            orderKey = QString("%1_%2").arg(QUICKORDER_SETTINGS_GROUP_ORDER).arg(index);
            QString orderValue = settings->value(orderKey).toString();
            group->orderlist.append(orderValue);
        }
        settings->endGroup();
        m_groupList.groupList.append(group);
    }
}

void RQuickOrderWidget::clearGroupsToFile()
{
    if(m_pUserSettings==NULL)
    {
        return ;
    }
    QSettings * settings = m_pUserSettings;

    settings->beginGroup(QUICKORDER_SETTINGS_GROUP);
    int groupCount = settings->value(QUICKORDER_SETTINGS_GROUP_TOTLECOUNT).toInt();
    settings->endGroup();
    settings->remove(QUICKORDER_SETTINGS_GROUP);

    for(int i=0;i<groupCount;i++)
    {
        QString section = QString("%1_%2").arg(QUICKORDER_SETTINGS_GROUP).arg(i);
        settings->remove(section);
    }
}

/*!
 * @brief 创建Ini文件
 */
void RQuickOrderWidget::createIniFile()
{
    QFileInfo fileInfo(m_filePath);
    if(!fileInfo.exists())
    {
        QFile file(m_filePath);
        if(!file.open(QFile::WriteOnly))
        {
            return ;
        }
        file.close();
    }
}

/*!
 * @brief 同步在编辑界面的操作到实际的内容
 */
void RQuickOrderWidget::syncTempToReal()
{
    m_allOrderList = m_allOrderList_Temp;
    m_groupList = m_groupList_Temp;
}

/*!
 * @brief 回滚编辑界面的操作到实际的内容，实际内容不改变
 */
void RQuickOrderWidget::rollbackTempFromReal()
{
    m_allOrderList_Temp = m_allOrderList;
    m_groupList_Temp = m_groupList;
}

/*!
 * @brief 检测用户对界面的操作
 * @return 如果操作产生了变化，返回true，没有变化返回false
 */
bool RQuickOrderWidget::checkUserOperations()
{

    return false;
}

