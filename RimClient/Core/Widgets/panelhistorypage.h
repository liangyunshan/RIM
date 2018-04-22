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
 */
#ifndef PANELHISTORYPAGE_H
#define PANELHISTORYPAGE_H

#include <QWidget>

#include "datastruct.h"

class PanelHistoryPagePrivate;

class PanelHistoryPage : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelHistoryPage)
public:
    explicit PanelHistoryPage(QWidget * parent = 0);
    ~PanelHistoryPage();

private slots:
    void viewDetail();
    void deleteContact();
    void setTopSession();
    void clearList();
    void sendInstantMessage();
    void removeSessionFromList();

    void loadHistoryList();
    void addHistoryItem(HistoryChatRecord record);

private:
    void createAction();
    void createHistoryItem(HistoryChatRecord &record);

private:
    PanelHistoryPagePrivate * d_ptr;

};

#endif // PANELHISTORYPAGE_H
