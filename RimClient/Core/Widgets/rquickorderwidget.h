/*!
 *  @brief     快捷命令类
 *  @details   支持生成默认快捷命令、添加删除快捷命令、保存为本地快捷命令文件
 *  @file      rquickorderwidget.h
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.08.08
 *  @copyright NanJing RenGu.
 */
#ifndef RQUICKORDERWIDGET_H
#define RQUICKORDERWIDGET_H

#include "widget.h"
#include "observer.h"

class RQuickOrderWidgetPrivate;
class QSettings;
class QStandardItem;

const char QUICKORDER_SETTINGS_ORDER [] = "ORDER";
const char QUICKORDER_SETTINGS_ORDER_COUNT [] = "count";
const char QUICKORDER_SETTINGS_ORDER_UNIT [] = "order";
const char QUICKORDER_SETTINGS_GROUP [] = "GROUP";
const char QUICKORDER_SETTINGS_GROUP_TOTLECOUNT [] = "count";
const char QUICKORDER_SETTINGS_GROUP_CURR [] = "current";
const char QUICKORDER_SETTINGS_GROUP_NAME [] = "groupname";
const char QUICKORDER_SETTINGS_GROUP_ORDERCOUNT [] = "ordercount";
const char QUICKORDER_SETTINGS_GROUP_ORDER [] = "order";
const char QUICKORDER_SETTINGS_GROUPLIST [] = "grouplist";

struct OrderGroup{
    QString groupName;
    QStringList orderlist;

    inline OrderGroup& operator = (const OrderGroup& group)
    {
        this->groupName = group.groupName;
        this->orderlist = group.orderlist;
        return *this;
    }
};

struct OrderGroupList{
    QString currGroupName;
    QList<OrderGroup*> groupList;

    inline OrderGroupList& operator = (const OrderGroupList& list)
    {
        this->currGroupName = list.currGroupName;
        this->groupList.clear();
        foreach(OrderGroup *group,list.groupList)
        {
            OrderGroup *newgroup=new OrderGroup;
            *newgroup = *group;
            this->groupList.append(newgroup);
        }
        return *this;
    }
};

struct AllOrderList{
    QStringList orderlist;

    inline AllOrderList& operator = (const AllOrderList& list)
    {
        this->orderlist = list.orderlist;
        return *this;
    }
};

class RQuickOrderWidget : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RQuickOrderWidget)
public:
    explicit RQuickOrderWidget(QWidget *parent = 0);
    ~RQuickOrderWidget();
    void onMessage(MessageType type);
    void addOrder(QString order=QString());
    int AllOrderCount();
    QStringList getCurrOrderList();
    QStringList getOrderList();
    QStringList getOpenGroupOrderList();
    QStringList getGroupsNameList();
    QStringList getAllGroupsOrderList();

    static RQuickOrderWidget * instance();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void setComplete(QStringList);

private slots:
    void clickOKButton(bool);
    void clickCancelButton(bool);
    void clickAddNewGroup(bool);
    void clickDeleteGroup(bool);
    void clickCloneGroup(bool);
    void clickMoveToCurrGroup(bool);
    void clickDeleteFromCurrGroup(bool);
    void clickMoveUpInCurrGroup(bool);
    void clickMoveDownInCurrGroup(bool);
    void updateOpenGroup();
    void addItemToAllOrder(bool);
    void deleteItemFromAllOrder(bool);
    void groupItemClick(const QModelIndex &index);

private:
    RQuickOrderWidgetPrivate * d_ptr;
    QSettings * m_pUserSettings;
    QString m_userHome;
    QString m_filePath;
    OrderGroupList m_groupList;
    OrderGroupList m_groupList_Temp;
    bool groupIsChanged;
    AllOrderList m_allOrderList;
    AllOrderList m_allOrderList_Temp;
    bool allOrderIsChanged;

private:
    void addNewGroup(QString groupName,QStringList orders);
    void addOrderToOpenGroup(QStringList orders);
    void resetGroupsListView();
    void setCurrGroup(QString groupName);
    void updateCurrOrderListView();
    void updateCurrOrderListView(QStringList orders);
    void updateAllOrderListView();

private:
    void creatDefaultOrderFile();
    void clearOrderFromAllOrder();
    void initOrderFilePath();
    void setOrderFilePath(QString path);
    bool orderIsExitedInOpenGroup(QString orderin);
    void createDefaultAllOrder();
    void createDefaultGroup();
    void readSettings();
    void updateAllOrderToFile();
    void readAllOrderFromFile();
    void updateGroupsToFile();
    void readGroupsFromFile();
    void clearGroupsToFile();
    void createIniFile();
    void syncTempToReal();
    void rollbackTempFromReal();

};

#endif // RQUICKORDERWIDGET_H
