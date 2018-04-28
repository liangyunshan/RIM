/*!
 *  @brief     历史聊天页面
 *  @details   显示登录用户历史与联系人交流的记录
 *             [1]聊天数据记录保存至本地的sql文件，
 *  @file      panelhistorypage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note 20180420:wey:增加从数据库读取会话列表
          20180422:wey:增加置顶操作，同步至数据库;
                       增加日期显示(根据最后一次聊天时间，显示日期或时间)
          20180423:wey:增加从item直接打开聊天、查看用户信息、删除联系人功能;
 */
#ifndef PANELHISTORYPAGE_H
#define PANELHISTORYPAGE_H

#include <QWidget>

#include "datastruct.h"

class PanelHistoryPagePrivate;
class ToolItem;

class PanelHistoryPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelHistoryPage)
public:
    explicit PanelHistoryPage(QWidget * parent = 0);
    ~PanelHistoryPage();

signals:
    void showChatWindow(ChatMessageType type,QString accountId);
    void showUserDetail(ChatMessageType type,QString accountId);
    void deleteUser(ChatMessageType type,QString accountId);

private slots:
    void loadHistoryList();
    void addHistoryItem(HistoryChatRecord record);

    void sendInstantMessage();
    void viewDetail();
    void deleteContact();
    void userDeleted(ChatMessageType type,QString groupId);
    void setTopSession();
    void removeSessionFromList();
    void clearList();

    void respItemDoubleClick(ToolItem * item);

private:
    void createAction();
    ToolItem *createHistoryItem(HistoryChatRecord &record);
    void removeHistoryItem(ToolItem * item);
    void setItemInfo(ToolItem * item,HistoryChatRecord & record);

private:
    PanelHistoryPagePrivate * d_ptr;
};

#endif // PANELHISTORYPAGE_H
