#include "shortcutsettings.h"

#include <QMap>

#include "commandfile.h"
#include "head.h"
#include "Util/rlog.h"

ShortcutSettings::ShortcutSettings(QObject *parent):
    QObject(parent)
{
    commandFile = new CommandFile("shortcuts.kms");
    if(!loadShortcuts())
    {
        RLOG_ERROR("can't read shortcuts file %s",commandFile->fileName().toLocal8Bit().data());
    }
}

ShortcutSettings::~ShortcutSettings()
{
    delete commandFile;
    commandFile = NULL;
}

QKeySequence ShortcutSettings::shortCut(Id id)
{
    if(items.size() > 0)
    {
        QList<ShortcutItem*>::iterator iter = items.begin();
        while(iter != items.end())
        {
            if((*iter)->m_id == id)
            {
                return (*iter)->m_key;
            }
            iter++;
        }
    }

    return QKeySequence();
}

void ShortcutSettings::addShortcut(Id id, QKeySequence &key)
{
    addShortcut(QString(id.data()),key);
}

void ShortcutSettings::addShortcut(QString id, QKeySequence &key)
{
    if(!contains(id.toLocal8Bit().data()))
    {
        ShortcutItem * item = new ShortcutItem;
        item->m_id = Id(id.toLocal8Bit().data());
        item->m_key = key;
        items.append(item);
    }
}

int ShortcutSettings::removeShortCut(Id id)
{
    QList<ShortcutItem*>::iterator iter = items.begin();
    while(iter != items.end())
    {
        if((*iter)->m_id == id)
        {
            items.removeOne((*iter));
            return 1;
        }
        iter++;
    }
    return 0;
}

bool ShortcutSettings::contains(QKeySequence key)
{
    QList<ShortcutItem*>::iterator iter = items.begin();
    while(iter != items.end())
    {
        if((*iter)->m_key == key)
        {
            return true;
        }
        iter++;
    }
    return false;
}

void ShortcutSettings::save()
{
    if(!commandFile->exportCommands(items))
    {
        RLOG_ERROR("Export shortcuts failed!");
    }
}

bool ShortcutSettings::loadShortcuts()
{
    QList<ShortcutItem*>::iterator iter = items.begin();
    while(iter != items.end())
    {
        delete *iter;
        iter++;
    }
    items.clear();

    QMap<QString, QKeySequence> result;

    if(commandFile->importCommands(result))
    {
        QMapIterator<QString, QKeySequence> it(result);
        while (it.hasNext())
        {
           it.next();

           ShortcutItem * item = new ShortcutItem();
           item->m_id = Id(it.key().toLocal8Bit().data());
           item->m_key = it.value();

           items.append(item);
        }
        return true;
    }
    return false;
}

bool ShortcutSettings::contains(Id id)
{
    QList<ShortcutItem*>::iterator iter = items.begin();
    while(iter != items.end())
    {
        if((*iter)->m_id == id)
        {
            return true;
        }
        iter++;
    }
    return false;
}
