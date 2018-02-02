/*!
 *  @brief     json聊天内容数据解析打包类
 *  @details   将聊天内容信息打包
 *  @author    XXX
 *  @version   1.0
 *  @date      2018.01.30
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef JSONRESOLVER_H
#define JSONRESOLVER_H

#include <QObject>
#include "datastruct.h"
class QXmlStreamReader;

class JsonResolver : public QObject
{
    Q_OBJECT
public:
    JsonResolver();

    TextUnit::ChatInfoUnit ReadJSONFile(QByteArray byteArray);
    QByteArray WriteJSONFile(TextUnit::ChatInfoUnit unit);
    int transUnitToQJsonDocument(const TextUnit::ChatInfoUnit unit, QJsonDocument &doc);

    QString imgStringTofilePath(QString &img);
    QString filePathToImgString(QString &path);

    int parseHtml(QString &out, const QString &html, TextUnit::ParseType type);


private:
    void readHtmlindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void readBodyindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void readPindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void readImgindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type);
    void readSpanindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType);
    void skipUnknownElement(QXmlStreamReader *xml);
};

#endif // JSONRESOLVER_H
