#include "basetextedit.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QMimeData>
#include <QCursor>
#include <QApplication>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QFile>
#include <QBuffer>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QDir>
#include "global.h"

#include <QDebug>

BaseTextEdit::BaseTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    this->installEventFilter(this);
}

bool BaseTextEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::Paste))
        {
            if(!this->isReadOnly())
            {
                QClipboard *clipboard = QApplication::clipboard();
                const QMimeData *mimeData = clipboard->mimeData();

                if(mimeData->hasHtml())
                {
                    qDebug() << "Ctrl + V mimeData->hasHtml()";
                    this->textCursor().insertHtml(mimeData->html());
                }
                else if(mimeData->hasText())
                {
                    qDebug() << "Ctrl + V mimeData->hasText()";
                    this->textCursor().insertText(mimeData->text());
                }
                if(mimeData->hasImage())
                {
                    qDebug() << "Ctrl + V mimeData->hasImage()";
                    QImage image = qvariant_cast<QImage>(mimeData->imageData());

                    insertCopyImage(image);
                }

                if(mimeData->hasColor())
                {
                    qDebug() << "Ctrl + V mimeData->hasColor()";
                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

//解析接收的json数据
TextUnit::ChatInfoUnit BaseTextEdit::ReadJSONFile(QByteArray byteArray)
{
    TextUnit::ChatInfoUnit unit;

    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);
    if((!doucment.isNull())
            && (jsonError.error == QJsonParseError::NoError))
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
QByteArray BaseTextEdit::WriteJSONFile(TextUnit::ChatInfoUnit unit)
{
    QJsonDocument document;
    this->transUnitToQJsonDocument(unit,document);
    QByteArray byteArray = document.toJson(QJsonDocument::Indented);
    return byteArray;
}

//将编辑区域内的聊天内容封装成json文件，返回QJsonDocument对象
int BaseTextEdit::transUnitToQJsonDocument(const TextUnit::ChatInfoUnit unit, QJsonDocument &doc)
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

//将内容打包成网络包 ChatInfoUnit
int BaseTextEdit::transTextToUnit(TextUnit::ChatInfoUnit &unit)
{
    QFont font = this->font();
    QColor color = this->textColor();
    QString contents = this->toChatFormaText();

    unit.user.id = TestUserId;
    unit.user.name = "Me";  //可以根据当前用户
    unit.user.head = ":/icon/resource/icon/person_1.png"; //用户头像
    unit.time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    unit.font.fontName = font.key();
    unit.font.fontSize = font.pointSize();
    unit.font.fontIsBold = font.bold();
    unit.font.fontIsItalic = font.italic();
    unit.font.fontIsUnderline = font.underline();
    unit.font.fontColorRGBA = color.rgba();
    unit.contents = contents;

    return 0;
}

//[1] 界面显示的数据->封装网络聊天数据 string->format
QString BaseTextEdit::toChatFormaText()
{
    QString ui_html = toHtml();
    QString contents ;
    parseHtml(contents,ui_html,TextUnit::Parase_Send);
    return contents;
}

//[2] 将网络封装的聊天数据->解析为界面可以显示的数据 format->string
void BaseTextEdit::insertChatFormatText(const QString &format)
{
    //解析数据，显示在界面
    QString contents ;
    parseHtml(contents,format,TextUnit::Parase_Recv);
    this->insertHtml(contents);
}

//将图片十六进制字符串转化为HTML格式路径
QString BaseTextEdit::imgStringTofilePath(QString &img)
{
 //   QString temp = QStandardPaths::writableLocation(QStandardPaths::TempLocation);

    QByteArray bb = QCryptographicHash::hash ( img.toLocal8Bit(), QCryptographicHash::Md5 );
    QString temp_img_filepath ;
    temp_img_filepath.append(bb.toHex());
    QString currPath = G_Temp_Picture_Path ;
    QString imgfilepath = currPath + "/" + temp_img_filepath + ".png";
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
QString BaseTextEdit::filePathToImgString(QString &path)
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

//解析html数据
//out:解析后的组装html,html:源html数据,type:解析类型
//一个显示正常的html格式如下：
//<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
//<html>
//<head>
//<meta name="qrichtext" content="1" />
//<style type="text/css"> p, li { white-space: pre-wrap; }</style>
//</head>
//<body style=" font-family:'幼圆'; font-size:18pt; font-weight:400; font-style:italic;">
//    <p style=" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;">
//    文字
//    <img src="file:///C:\Users\Shang\AppData\Local\Temp\(XO7V5IJS@{{%{TOKRVG(TA.gif" />
//    <img src="file:///C:\Users\Shang\AppData\Local\Temp\(XO7V5IJS@{{%{TOKRVG(TA.gif" />
//    <img src="file:///C:\Users\Shang\AppData\Roaming\Tencent\Users\893085067\TIM\WinTemp\RichOle\TA]I~IX)912AWJKH6{]7}6L.png" />
//    </p>
//</body>
//</html>
int BaseTextEdit::parseHtml(QString &out, const QString &html, TextUnit::ParseType type)
{
    if(type!=TextUnit::Parase_Send && type!=TextUnit::Parase_Recv)
    {
        return -1;
    }

    //
//! [0]
    QXmlStreamReader xml(html);
    QString newhtml = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n";
    newhtml += "<html>";
//    newhtml += "<head>";
//    newhtml += "<meta name=\"qrichtext\" content=\"1\" />";
//    newhtml += "<style type=\"text/css\">\n p, li { white-space: pre-wrap; }\n</style>";
//    newhtml += "</head>\n";
//! [0]

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

int BaseTextEdit::insertCopyImage(QImage &image)
{
    QByteArray bb = QCryptographicHash::hash ( QByteArray((char*)image.bits()), QCryptographicHash::Md5 );
    QString temp_img_filepath ;
    temp_img_filepath.append(bb.toHex());
    QString currPath = G_Temp_Picture_Path ;
    QString imgfilepath = currPath + "/" + temp_img_filepath + ".png";
    QFileInfo fileinfo(imgfilepath);
    if(!fileinfo.exists())
    {
        image.save(imgfilepath);
    }

    int width = this->viewport()->width() - 7;
    if (image.size().width() > width || image.size().height() > width)
    {
        image = image.scaled(width, width, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QTextImageFormat imageFormat;;
    imageFormat.setName(imgfilepath);
    this->textCursor().insertImage(imageFormat);

    return 0;
}

void BaseTextEdit::readHtmlindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
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

void BaseTextEdit::readBodyindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
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

void BaseTextEdit::readPindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
{
    //
    Q_ASSERT(xml->isStartElement() && xml->name() == "p");

    QString style = xml->attributes().value("style").toString();
    html += QString("\n<p style=\"%1\">").arg(style);

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

void BaseTextEdit::readImgindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
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

void BaseTextEdit::readSpanindexElement(QXmlStreamReader *xml, QString &html, TextUnit::ParseType type)
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

void BaseTextEdit::skipUnknownElement(QXmlStreamReader *xml)
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

void BaseTextEdit::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
    {
        qDebug()<<__FILE__<<__LINE__<<"\n"
               <<"e->mimeData()->hasFormat(\"text/uri-listb\")"
              <<"\n";
        e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
    }
}

void BaseTextEdit::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();

    foreach (QUrl u, urls) {
        qDebug()<<__FUNCTION__<<u.toString();
    }
    qDebug()<< urls.size();

    if(fileName.isEmpty())
        return;

    if(readFile(fileName))
        setWindowTitle(tr("%1 - %2").arg(fileName).arg("Drag File"));
}

bool BaseTextEdit::readFile(const QString &fileName) //读文件
{
//    QFile file(fileName);
//    if(!file.open(QFile::ReadOnly | QFile::Text))
//          return false;

//    QByteArray data;
//    data = file.readAll();
//    this->setText(QString::fromLocal8Bit(data));
    return true;
}
