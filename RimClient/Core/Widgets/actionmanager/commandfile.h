/*!
 *  @brief     命令解析文件
 *  @details   解析或保存快捷方式
 *  @file      commandfile.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.26
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef COMMANDFILE_H
#define COMMANDFILE_H

#include <QObject>
#include <QKeySequence>

struct ShortcutItem;

class CommandFile : public QObject
{
    Q_OBJECT
public:
    CommandFile(const QString &filename);

    bool importCommands(QMap<QString, QKeySequence> &) const;
    bool exportCommands(const QList<ShortcutItem *> &);

    QString fileName()const;

private:
    QString m_filename;
};

#endif // COMMANDFILE_H
