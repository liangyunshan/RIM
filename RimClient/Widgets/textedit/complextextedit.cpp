#include "complextextedit.h"
#include <QDateTime>
#include <QTextFrame>

#include <QDebug>

ComplexTextEdit::ComplexTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    m_ShowType = TextUnit::Type_NotOnlyHead;

    QFont frindFont("Times", 10, QFont::Bold);
    QFont meFont("Times", 10, QFont::Bold);

    m_Type_Default_Format.setForeground(Qt::black);
    m_Type_UserHead_Friend_Format.setForeground(Qt::blue);
    m_Type_UserHead_Friend_Format.setFont(frindFont);
    m_Type_UserHead_Me_Format.setForeground(Qt::darkGreen);
    m_Type_UserHead_Me_Format.setFont(meFont);
    m_Type_UserHead_Me_Format.setVerticalAlignment(QTextCharFormat::AlignMiddle);
    m_Type_ChatDetail_Format.setForeground(Qt::black);
    m_Type_RecordTime_Format.setForeground(Qt::lightGray);
    m_Type_Tip_Format.setForeground(Qt::darkBlue);
}

ComplexTextEdit::~ComplexTextEdit()
{

}

//设置界面需要显示的聊天记录的字体样式
void ComplexTextEdit::setChatFormat(const QTextCharFormat &format, TextUnit::BaseTextEditType type)
{
    switch(type)
    {
        case TextUnit::Type_Default:
        {
            m_Type_Default_Format = format;
        }
            break;
        case TextUnit::Type_UserHead_Friend:
        {
            m_Type_UserHead_Friend_Format = format;
        }
            break;
        case TextUnit::Type_UserHead_Me:
        {
            m_Type_UserHead_Me_Format = format;
        }
            break;
        case TextUnit::Type_ChatDetail:
        {
            m_Type_ChatDetail_Format = format;
        }
            break;
        case TextUnit::Type_RecordTime:
        {
            m_Type_RecordTime_Format = format;
        }
            break;
        case TextUnit::Type_Tip:
        {
            m_Type_Tip_Format = format;
        }
            break;
    }
    updateChatShow();
}

void ComplexTextEdit::setChatFont(const QFont &font, TextUnit::BaseTextEditType type)
{
    switch(type)
    {
        case TextUnit::Type_Default:
        {
            m_Type_Default_Format.setFont(font);
        }
            break;
        case TextUnit::Type_UserHead_Friend:
        {
            m_Type_UserHead_Friend_Format.setFont(font);
        }
            break;
        case TextUnit::Type_UserHead_Me:
        {
            m_Type_UserHead_Me_Format.setFont(font);
        }
            break;
        case TextUnit::Type_ChatDetail:
        {
            m_Type_ChatDetail_Format.setFont(font);
        }
            break;
        case TextUnit::Type_RecordTime:
        {
            m_Type_RecordTime_Format.setFont(font);
        }
            break;
        case TextUnit::Type_Tip:
        {
            m_Type_Tip_Format.setFont(font);
        }
            break;
        default:
        {
            return ;
        }
            break;
    }
    updateChatShow();
}

//添加对方的聊天内容
void ComplexTextEdit::insertFriendChatText(const TextUnit::InfoUnit record)
{
    this->moveCursor(QTextCursor::End);

    QFontMetrics fontMetrics(m_Type_UserHead_Friend_Format.font());
    int fontheight = fontMetrics.height();
    QImage image(record.user.head);
    image = image.scaled(fontheight,fontheight);
    this->textCursor().insertImage(image);

    QString me = QObject::tr(" %1 %2").arg(record.user.name).arg(record.time);
    this->textCursor().insertText(me ,m_Type_UserHead_Friend_Format);

    QTextFrameFormat frameFormat;                   //创建框架格式
    frameFormat.setBackground(Qt::white);          //设置背景色
    frameFormat.setMargin(9);                       //设置边距
    frameFormat.setPadding(9);                      //设置填充
    frameFormat.setBorder(1);                       //设置边界宽度
    this->textCursor().insertFrame(frameFormat);    //在光标处插入框架

    this->insertHtml(record.contents);

    this->moveCursor(QTextCursor::End);
    this->insertPlainText("\n");
}

//添加自己发送的聊天信息
void ComplexTextEdit::insertMeChatText(const TextUnit::InfoUnit record)
{
    this->moveCursor(QTextCursor::End);

    QFontMetrics fontMetrics(m_Type_UserHead_Me_Format.font());
    int fontheight = fontMetrics.height();
    QImage image(record.user.head);
    image = image.scaled(fontheight,fontheight);
    this->textCursor().insertImage(image);

    QString me = QObject::tr(" %1 %2").arg(record.user.name).arg(record.time);
    this->textCursor().insertText(me ,m_Type_UserHead_Me_Format);

    QTextFrameFormat frameFormat;                   //创建框架格式
    frameFormat.setBackground(Qt::color0);          //设置背景色
    frameFormat.setMargin(9);                       //设置边距
    frameFormat.setPadding(9);                      //设置填充
    frameFormat.setBorder(1);                       //设置边界宽度
    this->textCursor().insertFrame(frameFormat);    //在光标处插入框架

    this->insertHtml(record.contents);

    this->moveCursor(QTextCursor::End);
    this->insertPlainText("\n");

}

//添加标注提示信息
void ComplexTextEdit::insertTipChatText(const QString tip)
{
    this->moveCursor(QTextCursor::End);

    QTextFrameFormat frameFormat;                   //创建框架格式
    frameFormat.setBackground(Qt::gray);            //设置背景色
    frameFormat.setMargin(2);                       //设置边距
    frameFormat.setTopMargin(2);
    frameFormat.setBottomMargin(2);
    frameFormat.setLeftMargin(22);
    frameFormat.setPadding(2);                      //设置填充
    frameFormat.setBorder(0);                       //设置边界宽度
    this->textCursor().insertFrame(frameFormat);    //在光标处插入框架
    this->insertPlainText(tip);
}

void ComplexTextEdit::updateChatShow()
{

}

