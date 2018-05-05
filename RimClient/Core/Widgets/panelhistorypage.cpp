#include "panelhistorypage.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include <QDateTime>
#include <map>
#include <functional>
#include <ctime>
#include <memory>

#include "head.h"
#include "constants.h"
#include "toolbox/listbox.h"
#include "sql/sqlprocess.h"
#include "rsingleton.h"
#include "global.h"
#include "Util/imagemanager.h"
#include "user/user.h"
#include "messdiapatch.h"

#include "actionmanager/actionmanager.h"

class PanelHistoryPagePrivate : public GlobalData<PanelHistoryPage>
{
    Q_DECLARE_PUBLIC(PanelHistoryPage)

public:
    PanelHistoryPagePrivate(PanelHistoryPage * q):
        q_ptr(q)
    {
        initWidget();
    }
    void initWidget();

    PanelHistoryPage * q_ptr;
    ListBox * listBox;
    QWidget * contentWidget;

    std::map<ToolItem *,std::shared_ptr<HistoryChatRecord>> itemsMap;
    std::list<ToolItem *> topList;
};

void PanelHistoryPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    listBox = new ListBox(contentWidget);
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(listBox);
    contentWidget->setLayout(contentLayout);
}

PanelHistoryPage::PanelHistoryPage(QWidget *parent):
    d_ptr(new PanelHistoryPagePrivate(this)),
    QWidget(parent)
{
    createAction();

    d_ptr->listBox->setContextMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_HISTORY_TOOLBOX));

    connect(MessDiapatch::instance(),SIGNAL(createHisotryItem(HistoryChatRecord)),this,SLOT(addHistoryItem(HistoryChatRecord)));

    QTimer::singleShot(0,this,SLOT(loadHistoryList()));
}

PanelHistoryPage::~PanelHistoryPage()
{

}

/*!
 * @brief 通过历史记录发送即时信息
 * @details 若发送信息后，即时调整此信息在列表中的位置。
 */
void PanelHistoryPage::sendInstantMessage()
{
    MQ_D(PanelHistoryPage);
    if(d->listBox->selectedItem()){
        std::shared_ptr<HistoryChatRecord> record = d->itemsMap.at(d->listBox->selectedItem());
        emit showChatWindow(record->type,record->accountId);
    }
}

/*!
 * @brief 双击item快速打开聊天窗口
 * @param[in] item 双击的item信息
 */
void PanelHistoryPage::respItemDoubleClick(ToolItem *item)
{
    MQ_D(PanelHistoryPage);
    if(item){
        std::shared_ptr<HistoryChatRecord> record = d->itemsMap.at(item);
        emit showChatWindow(record->type,record->accountId);
    }
}

/*!
 * @brief 查看用户详细信息
 * @details 此操作交友PanelPersonPage处理，此处不重复处理
 */
void PanelHistoryPage::viewDetail()
{
    MQ_D(PanelHistoryPage);
    if(d->listBox->selectedItem()){
        std::shared_ptr<HistoryChatRecord> record = d->itemsMap.at(d->listBox->selectedItem());
        emit showUserDetail(record->type,record->accountId);
    }
}

/*!
 * @brief 删除指定用户
 * @details 此操作交友PanelPersonPage处理，此处不重复处理
 */
void PanelHistoryPage::deleteContact()
{
    MQ_D(PanelHistoryPage);
    if(d->listBox->selectedItem()){
        std::shared_ptr<HistoryChatRecord> record = d->itemsMap.at(d->listBox->selectedItem());
        emit deleteUser(record->type,record->accountId);
    }
}

/*!
 * @brief 好友删除成功后，从列表中移除对应的记录
 * @param[in] type 信息类型
 * @param[in] groupId 删除成功的好友ID
 */
void PanelHistoryPage::userDeleted(ChatMessageType type, QString groupId)
{
    MQ_D(PanelHistoryPage);

    if(d->listBox->selectedItem()){
        std::shared_ptr<HistoryChatRecord> record =  d->itemsMap.at(d->listBox->selectedItem());
        if(record->accountId == groupId){
            removeHistoryItem(d->listBox->selectedItem());
            return;
        }
    }

    auto findIndex = std::find_if(d->itemsMap.begin(),d->itemsMap.end(),[&groupId](const std::pair<ToolItem *,
                                  std::shared_ptr<HistoryChatRecord>> & item){
        return item.second->accountId == groupId;
    });

    if(findIndex != d->itemsMap.end()){
       removeHistoryItem((*findIndex).first);
    }
    Q_UNUSED(type);
}

void PanelHistoryPage::removeHistoryItem(ToolItem *item)
{
    MQ_D(PanelHistoryPage);
    if(item == nullptr)
        return;

    std::shared_ptr<HistoryChatRecord> record =  d->itemsMap.at(item);

    if(RSingleton<SQLProcess>::instance()->removeOneHistoryRecord(G_User->database(),record->accountId)){
        auto size = d->itemsMap.erase(item);
        if(size == 1){
            d->topList.remove(item);
            d->listBox->removeItem(item);
        }
    }
}

/*!
 * @brief 设置item置顶显示
 * @details 将item从list列表中暂时移除，根据topList的尺寸，重新插入到listbox中对应的位置
 */
void PanelHistoryPage::setTopSession()
{
    MQ_D(PanelHistoryPage);
    ToolItem * selectedItem = d->listBox->selectedItem();
    if(selectedItem){
        std::shared_ptr<HistoryChatRecord> record = d->itemsMap.at(selectedItem);
        if(RSingleton<SQLProcess>::instance()->topHistoryRecord(G_User->database(),record->accountId,!selectedItem->isTop())){
            if(selectedItem->isTop()){

                //此时items中包含了selectedItems，因此长度需要减1
                QList<ToolItem *> items = d->listBox->sortedItems();

                int index = -1;
                for(int i = d->topList.size();i < items.size();i++){
                    if(d->itemsMap.at(items.at(i))->dtime < record->dtime){
                        index = i - 1;
                        break;
                    }
                }

                if(index == -1)
                    index = items.size() - 1;

                if(d->listBox->reInsert(selectedItem,index)){
                    selectedItem->setTop(false);
                    d->topList.remove(selectedItem);
                }
            }else{
                if(d->listBox->reInsert(selectedItem,d->topList.size())){
                    d->topList.push_back(selectedItem);
                    selectedItem->setTop(true);
                }
            }
        }
    }
}

/*!
 * @brief 清空聊天列表
 * @details 1.删除所有的item；2.清空itemsMap容器
 */
void PanelHistoryPage::clearList()
{
    MQ_D(PanelHistoryPage);

    if(RSingleton<SQLProcess>::instance()->removeAllHistoryRecord(G_User->database())){
        d->listBox->removeAllItem();
        d->itemsMap.clear();
        d->topList.clear();
    }
}

/*!
 * @brief 用户状态改变，改变item在线状态
 * @param[in] status 联系人状态
 * @param[in] accountId 联系人账号
 */
void PanelHistoryPage::userStateChanged(OnlineStatus status, QString accountId)
{
    MQ_D(PanelHistoryPage);

    auto findIndex = std::find_if(d->itemsMap.begin(),d->itemsMap.end(),[&](const std::pair<ToolItem *,std::shared_ptr<HistoryChatRecord>> & item){
        return item.second->accountId == accountId;
    });

    if(findIndex != d->itemsMap.end()){
        (*findIndex).first->setStatus(status);
    }
}

/*!
 * @brief 联系人备注修改
 * @param[in] remarks 新备注名称
 * @param[in] accountId 待更新联系人账号
 */
void PanelHistoryPage::userInfoChanged(QString remarks, QString accountId)
{
    MQ_D(PanelHistoryPage);

    auto findIndex = std::find_if(d->itemsMap.begin(),d->itemsMap.end(),[&](const std::pair<ToolItem *,std::shared_ptr<HistoryChatRecord>> & item){
        return item.second->accountId == accountId;
    });

    if(findIndex != d->itemsMap.end()){
        (*findIndex).first->setName(remarks);
        (*findIndex).second->nickName = remarks;
        RSingleton<SQLProcess>::instance()->updateOneHistoryRecord(G_User->database(),*((*findIndex).second));
    }
}

/*!
 * @brief 将历史会话记录从会话列表中移除,同时从数据表HistoryChat表中移除对应记录
 */
void PanelHistoryPage::removeSessionFromList()
{
    MQ_D(PanelHistoryPage);
    removeHistoryItem(d->listBox->selectedItem());
}

/*!
 * @brief 加载历史会话列表
 */
void PanelHistoryPage::loadHistoryList()
{
    QList<HistoryChatRecord> recordList;
    if(RSingleton<SQLProcess>::instance()->loadChatHistoryChat(G_User->database(),recordList)){
        //TODO 20180422直接将谓语改成传递类成员函数
        std::for_each(recordList.begin(),recordList.end(),[&](HistoryChatRecord  record){
            createHistoryItem(record);
        });
//        std::for_each(recordList.begin(),recordList.end(),std::bind1st(std::mem_fun(&PanelHistoryPage::createHistoryItem), this));
    }
}

/*!
 * @brief 添加历史历史对话记录
 * @details 先查询是否存在当前accounid的数据记录信息，有则更新；无则创建;
 *          此item应该排列在除置顶item外最前位置。
 * @param[in] record 待添加记录的描述信息
 */
void PanelHistoryPage::addHistoryItem(HistoryChatRecord record)
{
    MQ_D(PanelHistoryPage);

    auto findIndex = std::find_if(d->itemsMap.cbegin(),d->itemsMap.cend(),[&record](const std::pair<ToolItem *,
                                  std::shared_ptr<HistoryChatRecord>> & item){
        return item.second->accountId == record.accountId;
    });

    ToolItem * item = nullptr;
    //find it
    if(findIndex != d->itemsMap.cend()){
        item = (*findIndex).first;

        record.isTop = item->isTop();
        setItemInfo(item,record);

        if(record.isTop){
            d->listBox->reInsert(item,0);
        }
        (*findIndex).second->dtime = record.dtime;
        RSingleton<SQLProcess>::instance()->updateOneHistoryRecord(G_User->database(),record);
    }else{
        item = createHistoryItem(record);
        RSingleton<SQLProcess>::instance()->addOneHistoryRecord(G_User->database(),record);
    }

    //加入至除置顶item外最靠前位置
    if(item && !record.isTop)
        d->listBox->reInsert(item,d->topList.size());
}

ToolItem * PanelHistoryPage::createHistoryItem(HistoryChatRecord &record)
{
    MQ_D(PanelHistoryPage);

    ToolItem * item = new ToolItem(NULL);
    connect(item,SIGNAL(itemDoubleClick(ToolItem*)),this,SLOT(respItemDoubleClick(ToolItem *)));
    item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_HISTORY_TOOLITEM));

    setItemInfo(item,record);

    std::shared_ptr<HistoryChatRecord> ptr = std::make_shared<HistoryChatRecord>(record);
    d->itemsMap.insert({item,ptr});

    d->listBox->addItem(item);
    if(record.isTop){
        if(d->listBox->reInsert(item,d->topList.size()))
            d->topList.push_back(item);
    }

    return item;
}

void PanelHistoryPage::setItemInfo(ToolItem *item, HistoryChatRecord &record)
{
    item->setName(record.nickName);
    item->setNickName(record.accountId);
    item->setDescInfo(record.lastRecord);
    item->setTop(record.isTop);

    QDateTime ctime = QDateTime::currentDateTime();
    QDateTime dtime = QDateTime::fromMSecsSinceEpoch(record.dtime);
    QDateTime yesterdayStart = ctime.addDays(-1);
    yesterdayStart.setTime(QTime(0,0,0));

    QDateTime yesterdatEnd = ctime.addDays(-1);
    yesterdatEnd.setTime(QTime(23,59,59));

    //当天
    if(ctime.date().day() - dtime.date().day()== 0){
        item->setDate(dtime.toString("hh:mm"));
    }else if(dtime >= yesterdayStart && dtime <= yesterdatEnd){
        item->setDate(QStringLiteral("昨天"));
    }else{
        item->setDate(dtime.toString("MM-dd"));
    }

    if(record.type == CHAT_C2C){
        item->setIcon(G_User->getIcon(record.systemIon,record.iconId));
        item->setStatus(record.status);
    }else{

    }
}

void PanelHistoryPage::createAction()
{
    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_HISTORY_TOOLBOX);

    QAction * clearListAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_CLEARLIST,this,SLOT(clearList()));
    clearListAction->setText(tr("Clear list"));

    menu->addAction(clearListAction);

    //创建联系人的右键菜单
    QMenu * historyMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_HISTORY_TOOLITEM);

    QAction * sendMessAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_SENDMESSAGE,this,SLOT(sendInstantMessage()));
    sendMessAction->setText(tr("Send Instant Message"));

    QAction * viewDetailAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_VIEWDETAIL,this,SLOT(viewDetail()));
    viewDetailAction->setText(tr("View Detail"));

    QAction * deleteAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_DELPERSON,this,SLOT(deleteContact()));
    deleteAction->setText(tr("Delete Friend"));

    QAction * stickSessionAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_STICKSESSION,this,SLOT(setTopSession()));
    stickSessionAction->setText(tr("Top session"));

    QAction * removeSessionAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_HISTORY_REMOVEFORMLIST,this,SLOT(removeSessionFromList()));
    removeSessionAction->setText(tr("Remove session"));

    historyMenu->addAction(sendMessAction);
    historyMenu->addSeparator();
    historyMenu->addAction(viewDetailAction);
    historyMenu->addAction(deleteAction);
    historyMenu->addSeparator();
    historyMenu->addSeparator();
    historyMenu->addAction(stickSessionAction);
    historyMenu->addAction(removeSessionAction);
}
