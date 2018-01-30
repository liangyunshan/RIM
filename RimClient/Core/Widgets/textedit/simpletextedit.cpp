#include "simpletextedit.h"
#include <QKeyEvent>
#include <QDebug>

#include "global.h"

SimpleTextEdit::SimpleTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    setInputColor(QColor(Qt::black));
    setInputFont(QFont(tr("wryh"), 12, QFont::Bold));

    connect(this,SIGNAL(textChanged()),this,SLOT(slot_TextChanged()));
}

SimpleTextEdit::~SimpleTextEdit()
{

}

void SimpleTextEdit::setInputColor(QColor color)
{
    m_fontcolor = color;
    this->setTextColor(m_fontcolor);
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
                    emit sigEnter();
                }
            }
        }
    }
    BaseTextEdit::keyPressEvent(event);
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


