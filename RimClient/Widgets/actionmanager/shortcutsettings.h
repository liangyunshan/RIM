/*!
 *  @brief     快捷键管理
 *  @details   支持对保存、加载本地的快捷键设置
 *  @file      shortcutsettings.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.26
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef SHORTCUTSETTINGS_H
#define SHORTCUTSETTINGS_H

#include <QObject>
#include <QKeySequence>

#include "id.h"

class CommandFile;

struct ShortcutItem
{
    Id m_id;
    QKeySequence m_key;
};

class ShortcutSettings : public QObject
{
    Q_OBJECT
public:
    ShortcutSettings(QObject * parent = 0);
    ~ShortcutSettings();

    QKeySequence shortCut(Id id);
    void addShortcut(Id id,QKeySequence  &key);
    void addShortcut(QString id,QKeySequence &key);

    void save();

private:
    bool loadShortcuts();

private:
    bool contains(Id id);

    CommandFile * commandFile;

    QList<ShortcutItem *> items;
};

#endif // SHORTCUTSETTINGS_H
