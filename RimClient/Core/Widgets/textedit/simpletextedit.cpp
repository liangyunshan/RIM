#include "simpletextedit.h"
#include <QKeyEvent>
#include <QDebug>

#include "global.h"

SimpleTextEdit::SimpleTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    setInputColor(QColor(Qt::black));
    setInputFont(QFont(tr("wryh"), 12, QFont::Bold));

    b_isSendStatus = false;

    connect(this,SIGNAL(textChanged()),this,SLOT(slot_TextChanged()));
    this->clear();
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

void SimpleTextEdit::clear()
{
    QTextEdit::clear();
    updateInputInfo();
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


