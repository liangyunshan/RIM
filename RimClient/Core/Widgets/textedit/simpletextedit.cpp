#include "simpletextedit.h"
#include <QKeyEvent>
#include <QRegExp>
#include <QDomDocument>
#include <QDomElement>

#include "global.h"
#include "user/user.h"

#include <QDebug>

SimpleTextEdit::SimpleTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    setInputColor(QColor(Qt::black));
    setInputFont(QFont(tr("wryh"), 12, QFont::Bold));

    b_isSendStatus = false;

    connect(this,SIGNAL(textChanged()),this,SLOT(slot_TextChanged()));
    this->clear();
    imgTypes<<"bmp"<<"jpeg"<<"jpg"<<"png"<<"tiff"<<"gif"<<"pcx"<<"tga"<<"exif"<<"fpx"<<"svg";
}

SimpleTextEdit::~SimpleTextEdit()
{

}

void SimpleTextEdit::setInputColor(QColor color)
{
    m_fontcolor = color;
    this->setInputTextColor(m_fontcolor);
}

void SimpleTextEdit::setInputFont(QFont font)
{
    m_inputFont = font;
    this->setFont(m_inputFont);
}

void SimpleTextEdit::updateInputInfo()
{
    this->setTextColor(m_fontcolor);
    this->setFont(m_inputFont);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::BrightText,m_fontcolor);
    setPalette(palette);
}

void SimpleTextEdit::keyPressEvent(QKeyEvent *event)
{
    if(event->key())
    {
        if(event->modifiers() == Qt::NoModifier)
        {
            if(event->key() == Qt::Key_Return)
            {
                if(G_mIsEnter == Qt::Key_Return)
                {
                    b_isSendStatus = true;
                    emit sigEnter();
                }
            }
        }
        else if(event->modifiers() == Qt::ControlModifier)
        {
            if(event->key() == Qt::Key_Return)
            {
                if(G_mIsEnter == (Qt::Key_Control+Qt::Key_Return))
                {
                    b_isSendStatus = true;
                    emit sigEnter();
                }
            }
        }

        if(this->toPlainText().isEmpty())
        {
            if(event->key() == Qt::Key_Backspace)
            {
                this->clear();
            }
        }
    }
    BaseTextEdit::keyPressEvent(event);

    if(b_isSendStatus)
    {
        b_isSendStatus = false;
        this->clear();
    }
}

/*!
 * \brief SimpleTextEdit::clear 清空输入框的内容及收回相关内存
 */
void SimpleTextEdit::clear()
{
    QTextEdit::clear();
    updateInputInfo();
    BaseTextEdit::clearInputImg();
}

/*!
 * @brief SimpleTextEdit::extractPureHtml
 * @details 提取消息输入框中HTML内容，移除其中全部样式等无关内容
 * @param  [in]content:调用者传入参数,保存处理后的内容
 * @return 无
 */
void SimpleTextEdit::extractPureHtml(QString &content)
{
    if(this->toPlainText().isEmpty())
    {
        return;
    }
    QString wholeHTML = this->toHtml().trimmed();
    QString removeNewLine = wholeHTML.replace("\n","");
    //去除body之外的无关内容
    QRegExp wholeRegexp("<body.*</body>");
    wholeRegexp.setMinimal(true);
    int wholePos = 0;
    while((wholePos = wholeRegexp.indexIn(removeNewLine,wholePos)) !=-1)
    {
        wholePos += wholeRegexp.matchedLength();
        removeNewLine = removeNewLine.replace(wholePos,removeNewLine.length()-wholePos,"");
        removeNewLine = removeNewLine.replace(0,removeNewLine.length()-wholeRegexp.matchedLength(),"");
    }

    //获取body标签中的样式属性替换到p标签中样式属性
    QDomDocument domDoc;
    domDoc.setContent(removeNewLine);
    QString rootName = domDoc.documentElement().tagName();
    QString style = QString("");
    if(rootName == "body")
    {
        style = domDoc.documentElement().attribute("style");
        QDomNodeList nodes = domDoc.documentElement().childNodes();
        for(int index=0;index<nodes.count();index++)
        {
            if(nodes.at(index).nodeName()=="p"&&nodes.at(index).isElement())
            {
                nodes.at(index).toElement().setAttribute("style",style);
            }
        }
    }
    removeNewLine = domDoc.toString();

    //去除body标签，保留body标签内部包裹的内容
    QRegExp startRegexp("<body.*>");
    startRegexp.setMinimal(true);
    int stylePos = 0;
    while((stylePos = startRegexp.indexIn(removeNewLine,stylePos)) !=-1)
    {
        stylePos += startRegexp.matchedLength();
        removeNewLine.remove(0,stylePos);
    }
    content = removeNewLine.replace("</body>","").replace("\n","");
}

void SimpleTextEdit::slot_TextChanged()
{
    if(this->textColor() != m_fontcolor)
    {
        this->setTextColor(m_fontcolor);
    }
    if(this->font() != m_inputFont)
    {
        this->setFont(m_inputFont);
    }
}

