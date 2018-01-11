#include "simpletextedit.h"

SimpleTextEdit::SimpleTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    QColor color(Qt::blue);
    m_Fontcolor = color;
    this->setTextColor(m_Fontcolor);
    connect(this,SIGNAL(textChanged()),this,SLOT(slot_TextChanged()));
}

SimpleTextEdit::~SimpleTextEdit()
{

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


