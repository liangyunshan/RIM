#include "actionmanager.h"

#include <QDebug>

#include "util/rsingleton.h"

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
    if(actionManager == NULL)
    {
        actionManager = new ActionManager();
    }
    return actionManager;
}

ActionManager::~ActionManager()
{

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

