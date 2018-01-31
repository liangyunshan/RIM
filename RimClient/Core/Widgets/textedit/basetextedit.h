/*!
 *  @brief     基础文本编辑
 *  @details
 *  @file      basetextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BASETEXTEDIT_H
#define BASETEXTEDIT_H

#include <QTextEdit>
#include "datastruct.h"

class QTextStream;
class QXmlStreamReader;
class BaseTextEdit : public QTextEdit
{
public:
    BaseTextEdit(QWidget * parent = 0 );

    int transTextToUnit(TextUnit::ChatInfoUnit &unit);

    QString toChatFormaText();
    void insertChatFormatText(const QString &text);

    int insertCopyImage(QImage &image);

protected:
    bool eventFilter(QObject *, QEvent *);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

};

#endif // BASETEXTEDIT_H
