#include "simpletextedit.h"
#include <QKeyEvent>
#include <QDebug>

#include "global.h"

SimpleTextEdit::SimpleTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    QColor color(Qt::blue);
    m_Fontcolor = color;
    this->setTextColor(m_Fontcolor);
    this->setStyleSheet("");
    connect(this,SIGNAL(textChanged()),this,SLOT(slot_TextChanged()));
}

SimpleTextEdit::~SimpleTextEdit()
{

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
    return BaseTextEdit::keyPressEvent(event);
}

void SimpleTextEdit::clear()
{
    QTextEdit::clear();
}


void SimpleTextEdit::slot_TextChanged()
{
    if(this->textColor() != m_Fontcolor)
    {
        this->setTextColor(m_Fontcolor);
    }
}


