/*!
 *  @brief     群信息界面
 *  @details   显示登录用户群信息
 *  @file      panelgrouppage.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @change
 *      date:20180122 content:添加群分组重命名 name:LYS
 *      date:20180202 content:添加增加群分组功能 name:LYS
 */
#ifndef PANELGROUPPAGE_H
#define PANELGROUPPAGE_H

#include <QWidget>
#include "observer.h"
#include "protocoldata.h"

class PanelGroupPagePrivate;
class ToolItem;
class ToolPage;

class PanelGroupPage : public QWidget , public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PanelGroupPage)
public:
    explicit PanelGroupPage(QWidget *parent = 0);
    ~PanelGroupPage();

    void onMessage(MessageType type);

private slots:
    void searchGroup();
    void newGroup();
    void addGroups();

    void renameGroup();
    void deleteGroup();

    void sendMessage();
    void viewDetail();
    void modifyGroupInfo();
    void exitGroup();

public slots:
    void renameEditFinished();
    void updateGroupActions(ToolPage *);
    void moveGroupTo();

private:
    void createAction();
    void addGroupAndCluster();
    void clearTargetGroup(const QString id);

private:
    PanelGroupPagePrivate * d_ptr;

};

#endif // PANELGROUPPAGE_H
