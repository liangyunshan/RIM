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

namespace TextUnit {
    enum ShowType{
        Type_OnlyHead = 0,
        Type_NotOnlyHead ,
    };
    enum BaseTextEditType{
        Type_Default = 0,           //编辑器类型:默认
        Type_UserHead_Friend,       //编辑器类型:用户头
        Type_UserHead_Me ,          //编辑器类型:用户头
        Type_ChatDetail ,           //编辑器类型:聊天内容
        Type_RecordTime ,           //编辑器类型:记录时间
        Type_Tip                    //编辑器类型:提示信息
    };

    struct UserInfo{
        QString name;
        QString head;
    };//用户描述

    const char ChatFormatType_Text[] = "Text";
    const char ChatFormatType_Emoj[] = "Emoj";
    const char ChatFormatType_Image[] = "Image";

    struct UserChatFont{
        QString fontName;
        qint32  fontSize;
        bool    fontIsBold;
        bool    fontIsItalic;
        bool    fontIsUnderline;
        qint64  fontColorRGBA;

        UserChatFont (){
            fontName = "Unknown";
            fontSize = 0;
            fontIsBold = false;
            fontIsItalic = false;
            fontIsUnderline = false;
            fontColorRGBA = 0x00000000;
        }
    };//聊天内容字体描述

    struct ChatInfoUnit{
        UserInfo user;
        QString time;
        UserChatFont font;
        QString contents;

        ChatInfoUnit (){
            user.name = "Unknown";
            user.head = "Unknown";
            time = "";
            contents = "";
        }
    };//一条聊天记录描述

    enum ParseType{
        Parase_Send,    //解析本地的控件的html
        Parase_Recv     //解析网络端发送来的html
    };//聊天数据包解析类型,替换图形为字符串数据

}

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

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    void readHtmlindexElement(QXmlStreamReader *reader, QString &html, TextUnit::ParseType);
    void readBodyindexElement(QXmlStreamReader *reader, QString &html, TextUnit::ParseType);
    void readPindexElement(QXmlStreamReader *reader, QString &html, TextUnit::ParseType);
    void skipUnknownElement(QXmlStreamReader *reader);

};

#endif // BASETEXTEDIT_H
