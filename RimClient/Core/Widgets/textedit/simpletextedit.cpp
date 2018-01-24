#include "simpletextedit.h"
#include <QKeyEvent>
#include <QDebug>

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
   switch(event->key())
   {
        case Qt::Key_Return:
            emit sigEnter();
            break;
        default:
        BaseTextEdit::keyPressEvent(event);
   }
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


