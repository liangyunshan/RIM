/*!
 *  @brief     工具按钮管理
 *  @details   管理所创建的工具按钮，根据ID返回对应的工具按钮
 *  @file      toolactionmanager.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.16
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20171219:wey:添加创建和获取action;
 */
#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QObject>
#include <QMenu>
#include <QAction>
#include <QMap>
#include <QKeySequence>
#include "id.h"
#include "widgets/widget/rtoolbutton.h"

class QToolButton;
class LoginDialog;
class RToolButton;

class ActionManager : public QObject
{
    Q_OBJECT
public:
    static ActionManager * instance();
    ~ActionManager();

    RToolButton * createToolButton(Id id, QObject *reciver = NULL, const char * slot = NULL, bool isToggled = false);
    RToolButton * toolButton(Id id);

    QAction * createAction(Id id, QObject *reciver = NULL, const char * slot = NULL, bool isToggled = false);
    QAction * action(Id id);

    void registShortAction(QAction *action, QKeySequence defaultKey);
    QKeySequence shortcut(Id id,QKeySequence defaultKey);

    QMenu * createMenu(Id id);
    QMenu * menu(Id id);

private:
    ActionManager(QObject * parent = 0);

    static ActionManager * actionManager;

    QMap<Id,RToolButton *> toolButtons;
    QMap<Id,QAction *> actions;
    QMap<Id,QMenu *> menus;

    friend class LoginDialog;

};

#endif // TOOLACTIONMANAGER_H
