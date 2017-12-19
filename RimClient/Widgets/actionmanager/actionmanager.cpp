#include "actionmanager.h"

#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QDebug>

template<class T,class P>
bool contains(QMap<T,P*> & map,const T &id)
{
    QMapIterator<T,P *> iterator(map);
    while(iterator.hasNext())
    {
        iterator.next();
        if(iterator.key() == id)
        {
            return true;
        }
    }
    return false;
}

ActionManager * ActionManager::actionManager = NULL;

ActionManager::ActionManager(QObject *parent):
    QObject(parent)
{
    actionManager = this;
}

ActionManager *ActionManager::instance()
{
    return actionManager;
}

ActionManager::~ActionManager()
{

}

RToolButton *ActionManager::createToolButton(Id id,QObject * reciver, const char *slot, bool isToggled)
{
    if(contains<Id,RToolButton>(toolButtons,id))
    {
        return toolButtons.value(id);
    }
    RToolButton * toolButt = new RToolButton();
    toolButt->setObjectName(id.data());

    if(reciver)
    {
        toolButt->setCheckable(isToggled);
        if(isToggled)
        {
            connect(toolButt,SIGNAL(toggled(bool)),reciver,slot);
        }
        else
        {
            connect(toolButt,SIGNAL(pressed()),reciver,slot);
        }
    }

    toolButtons.insert(id,toolButt);

    return toolButt;
}

RToolButton *ActionManager::toolButton(Id id)
{
    if(contains<Id,RToolButton>(toolButtons,id))
    {
        return toolButtons.value(id);
    }

    return NULL;
}

QAction *ActionManager::createAction(Id id, QObject *reciver, const char *slot, bool isToggled)
{
    if(contains<Id,QAction>(actions,id))
    {
        return actions.value(id);
    }
    QAction * action = new QAction();
    action->setObjectName(id.data());

    if(reciver)
    {
        action->setCheckable(isToggled);
        connect(action,SIGNAL(triggered(bool)),reciver,slot);
    }

    actions.insert(id,action);

    return action;
}

QAction *ActionManager::action(Id id)
{
    if(contains<Id,QAction>(actions,id))
    {
        return actions.value(id);
    }

    return NULL;
}

QMenu *ActionManager::createMenu(Id id)
{
    if(contains<Id,QMenu>(menus,id))
    {
        return menus.value(id);
    }
    QMenu * menu = new QMenu();
    menu->setObjectName(id.data());

    menus.insert(id,menu);

    return menu;
}

QMenu *ActionManager::menu(Id id)
{
    if(contains<Id,QMenu>(menus,id))
    {
        return menus.value(id);
    }

    return NULL;
}

