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

    TextUnit::ChatInfoUnit ReadJSONFile(QByteArray byteArray);
    QByteArray WriteJSONFile(TextUnit::ChatInfoUnit unit);
    int transTextToUnit(TextUnit::ChatInfoUnit &unit);
    int transUnitToQJsonDocument(const TextUnit::ChatInfoUnit unit, QJsonDocument &doc);

    QString imgStringTofilePath(QString &img);
    QString filePathToImgString(QString &path);
    QString toChatFormaText();
    void insertChatFormatText(const QString &text);

    int parseHtml(QString &out, const QString &html, TextUnit::ParseType type);
    int insertCopyImage(QImage &image);

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    void readHtmlindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void readBodyindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void readPindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void readImgindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type);
    void readSpanindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type);
    void skipUnknownElement(QXmlStreamReader *xml);

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

};

#endif // BASETEXTEDIT_H
