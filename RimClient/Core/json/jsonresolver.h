/*!
 *  @brief     json聊天内容数据解析打包类
 *  @details   将聊天内容信息打包
 *  @author    SC
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

    ChatInfoUnit ReadJSONFile(QByteArray byteArray);
    QByteArray WriteJSONFile(ChatInfoUnit unit);
    int transUnitToQJsonDocument(const ChatInfoUnit unit, QJsonDocument &doc);

    QString imgStringTofilePath(QString &img);
    QString filePathToImgString(QString &path);

    int parseHtml(QString &out, const QString &html, ParseType type);


private:
    void readHtmlindexElement(QXmlStreamReader *xml, QString &html, ParseType);
    void readBodyindexElement(QXmlStreamReader *xml, QString &html, ParseType);
    void readPindexElement(QXmlStreamReader *xml, QString &html, ParseType);
    void readImgindexElement(QXmlStreamReader *xml, QString &html, ParseType type);
    void readSpanindexElement(QXmlStreamReader *xml, QString &html, ParseType);
    void skipUnknownElement(QXmlStreamReader *xml);
};

#endif // JSONRESOLVER_H
