#include "jsonresolver.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

#include <QApplication>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QFile>
#include <QBuffer>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QDir>
#include <QFont>
#include <QImage>

#include "global.h"

JsonResolver::JsonResolver()
{

}

TextUnit::ChatInfoUnit JsonResolver::ReadJSONFile(QByteArray byteArray)
{
    TextUnit::ChatInfoUnit unit;

    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);
    qDebug()<<jsonError.errorString();
    if((!doucment.isNull()) && (jsonError.error == QJsonParseError::NoError))
    {
        if(doucment.isObject())
        {
            QJsonObject obj = doucment.object();
            if(obj.contains("UserID"))
            {
                unit.user.id = obj.value("UserID").toInt();
            }
            if(obj.contains("UserName"))
            {
                unit.user.name = obj.value("UserName").toString();
            }
            if(obj.contains("UserHead"))
            {
                unit.user.head = obj.value("UserHead").toString();
            }
            if(obj.contains("RecordTime"))
            {
                unit.time = obj.value("RecordTime").toString();
            }
            if(obj.contains("RecordFont"))
            {
                QJsonValue font_Value = obj.value("RecordFont");
                if(font_Value.isObject())
                {
                    QJsonObject font_obj = font_Value.toObject();
                    if(font_obj.contains("FontName"))
                    {
                        unit.font.fontName = font_obj.value("FontName").toString();
                    }
                    if(font_obj.contains("FontSize"))
                    {
                        unit.font.fontSize = font_obj.value("FontSize").toInt();
                    }
                    if(font_obj.contains("FontIsBold"))
                    {
                        unit.font.fontIsBold = font_obj.value("FontIsBold").toBool();
                    }
                    if(font_obj.contains("FontIsItalic"))
                    {
                        unit.font.fontIsItalic = font_obj.value("FontIsItalic").toBool();
                    }
                    if(font_obj.contains("FontIsUnderline"))
                    {
                        unit.font.fontIsUnderline = font_obj.value("FontIsUnderline").toBool();
                    }
                    if(font_obj.contains("FontColorRGBA"))
                    {
                        unit.font.fontColorRGBA = font_obj.value("FontColorRGBA").toInt();
                    }
                }
            }
            if(obj.contains("RecordContents"))
            {
                unit.contents = obj.value("RecordContents").toString();
            }
        }
    }

    return unit;
}

//编辑json数据发送
QByteArray JsonResolver::WriteJSONFile(TextUnit::ChatInfoUnit unit)
{
    QJsonDocument document;
    this->transUnitToQJsonDocument(unit,document);
    QByteArray byteArray = document.toJson(QJsonDocument::Indented);
    return byteArray;
}

//将编辑区域内的聊天内容封装成json文件，返回QJsonDocument对象
int JsonResolver::transUnitToQJsonDocument(const TextUnit::ChatInfoUnit unit, QJsonDocument &doc)
{
    // 构建字体对象
    QJsonObject font;
    font.insert("FontName",unit.font.fontName);
    font.insert("FontSize",unit.font.fontSize);
    font.insert("FontIsBold",unit.font.fontIsBold);
    font.insert("FontIsItalic",unit.font.fontIsItalic);
    font.insert("FontIsUnderline",unit.font.fontIsUnderline);
    font.insert("FontColorRGBA",unit.font.fontColorRGBA);

    // 构建 Json 对象
    QJsonObject Unit;
    Unit.insert("UserID", unit.user.id);
    Unit.insert("UserName", unit.user.name);
    Unit.insert("UserHead", unit.user.head);
    Unit.insert("RecordTime", unit.time);
    Unit.insert("RecordFont", QJsonValue(font));
    Unit.insert("RecordContents", unit.contents);

    // 构建 JSON 文档
    doc.setObject(Unit);
    return 0;
}

//解析html数据: out:解析后的组装html,html:源html数据,type:解析类型
int JsonResolver::parseHtml(QString &out, const QString &html, TextUnit::ParseType type)
{
    if(type!=TextUnit::Parase_Send && type!=TextUnit::Parase_Recv)
    {
        return -1;
    }

    QXmlStreamReader xml(html);
    QString newhtml = "<!DOCTYPE HTML>\n";
    newhtml += "<html>";

    if (xml.readNextStartElement()) {
        if (xml.name() == "html")
        {
            readHtmlindexElement(&xml,newhtml,type);
        }
        else
            xml.raiseError(QObject::tr("The file is not an XBEL version 1.0 file."));
    }

    newhtml += "</html>";
    out = newhtml;

    if (xml.hasError()) {
        return !xml.error();;
    }

    return 0;
}

void JsonResolver::readHtmlindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
{
    Q_ASSERT(xml->isStartElement() && xml->name() == "html");

    while (xml->readNextStartElement()) {
        if (xml->name() == "body")
        {
            QString style = xml->attributes().value("style").toString();
            html += QString("<body style=\"%1\">").arg(style);
            readBodyindexElement(xml,html,type);
        }
        else if(xml->name() == "p")
        {
            readPindexElement(xml,html,type);
        }
        else
        {
            xml->skipCurrentElement();
        }
    }

    html += "</body>";
}

void JsonResolver::readBodyindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
{
    //
    Q_ASSERT(xml->isStartElement() && xml->name() == "body");

    while (xml->readNextStartElement()) {
        if (xml->name() == "p")
        {
            readPindexElement(xml,html,type);
        }
        else if(xml->name() == "span")
        {

        }
        else
        {
            xml->skipCurrentElement();
        }
    }
    //
}

void JsonResolver::readPindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
{
    //
    Q_ASSERT(xml->isStartElement() && xml->name() == "p");

//    QString style = xml->attributes().value("style").toString();
//    html += QString("\n<p style=\"%1\">").arg(style);
    html += QString("\n<p >");

    while (xml->readNextStartElement()) {
        if (xml->name() == "img")
        {
            readImgindexElement(xml,html,type);
        }
        else if(xml->name() == "span")
        {
            readSpanindexElement(xml,html,type);
        }
        else
        {
            xml->skipCurrentElement();
        }
    }
    html += "</p>";
    //
}

void JsonResolver::readImgindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
{

    Q_ASSERT(xml->isStartElement() && xml->name() == "img");

    QString filepath = xml->attributes().value("","src").toString();
    QString image ;
    if(type == TextUnit::Parase_Send)
    {
        image = filePathToImgString(filepath);
    }
    else
    {
        image = "file:///";
        QString imgab = imgStringTofilePath(filepath);
        imgab = QDir::toNativeSeparators(imgab);
        image += imgab;
    }
    //src=\"%1\  ,不要有空格
    QString newhtml = QString("<img src=\"%1\" />").arg(image);
    html += newhtml;

    xml->skipCurrentElement();
}

void JsonResolver::readSpanindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType)
{
    //
    Q_ASSERT(xml->isStartElement() && xml->name() == "span");

    QString style = xml->attributes().value("style").toString();
    QString span = xml->readElementText();

    span = span.toHtmlEscaped();

    QString span_format = QString("<span style=\"%1\">%2</span>").arg(style).arg(span);
    html += span_format;
    //
}

void JsonResolver::skipUnknownElement(QXmlStreamReader *xml)
{
    xml->readNext();
    while (!xml->atEnd())
    {
        if(xml->isEndElement())
        {
            xml->readNext();
            break;
        }

        if(xml->isStartElement())
        {
            skipUnknownElement(xml);
        } else
        {
            xml->readNext();
        }
    }
}

//将图片十六进制字符串转化为HTML格式路径
QString JsonResolver::imgStringTofilePath(QString &img)
{
 //   QString temp = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    QByteArray bb = QCryptographicHash::hash ( img.toLocal8Bit(), QCryptographicHash::Md5 );
    QString temp_img_filepath ;
    temp_img_filepath.append(bb.toHex());
    QString imgfilepath = G_Temp_Picture_Path + "/" + temp_img_filepath + ".png";
    QByteArray read = img.toLatin1();
    QString rStr = QString::fromLatin1(read.data(), read.size());
    QByteArray readCompressed = QByteArray::fromHex(rStr.toLocal8Bit());
    QByteArray readDecompressed = qUncompress(readCompressed);

    QImage readImg;
    readImg.loadFromData(readDecompressed);
    if (readImg.isNull())
        qDebug("The image is null. Something failed.");
    readImg.save(imgfilepath);

    QString path = imgfilepath;
    return path;
}

//将html中文件路径对应的图片资源转化为字符串
QString JsonResolver::filePathToImgString(QString &path)
{

    QString imageFile = path;
    QFileInfo fileinfo(path);
    if(!fileinfo.exists())
    {
        imageFile = path.right(path.length()-8);
    }

    QImage image(imageFile);
    QByteArray ba;
    QBuffer buf(&ba);
    image.save(&buf, "png");

    QByteArray compressed = qCompress(ba, 1); // better just open file with QFile, load data, compress and toHex?
    QByteArray hexed = compressed.toHex();
    QString str(hexed);
    return str;
}
