#include "rlineedit.h"

#include <QEvent>
#include <QStyle>
#include <QMetaEnum>
#include <QKeyEvent>
#include <QDebug>

RLineEdit::RLineEdit(QWidget *parent):
    QLineEdit(parent)
{
    setFocusIn(ToolItem::Item_Normal);
}

RLineEdit::~RLineEdit()
{

}

bool RLineEdit::event(QEvent *eve)
{
    if(eve->type() == QEvent::FocusIn)
    {
        setFocusIn(ToolItem::Item_FocusIn);
        setFocus();
        originContent = text();
        return true;
    }
    else if(eve->type() == QEvent::FocusOut)
    {
        setFocusIn(ToolItem::Item_FocusOut);
        clearFocus();
        if(text() != originContent)
        {
            emit contentChanged(text());
        }
        return true;
    }
    else if(eve->type() == QEvent::Enter)
    {
        if(itemState != ToolItem::Item_FocusIn)
        {
            setFocusIn(ToolItem::Mouse_Enter);
        }
        return true;
    }
    else if(eve->type() == QEvent::Leave)
    {
        if(itemState != ToolItem::Item_FocusIn)
        {
           setFocusIn(ToolItem::Mouse_Leave);
           return true;
        }
    }
    else if(eve->type() == QEvent::KeyRelease)
    {
        QKeyEvent * keyEvent = dynamic_cast<QKeyEvent *>(eve);
        if(keyEvent && keyEvent->key() == Qt::Key_Return)
        {
            setFocusIn(ToolItem::Item_FocusOut);
            clearFocus();
            if(text() != originContent)
            {
                emit contentChanged(text());
            }
        }
    }
    return QLineEdit::event(eve);
}

void RLineEdit::setFocusIn(ToolItem::ItemState state)
{
    itemState = state;
    QMetaEnum metaEnum = QMetaEnum::fromType<ToolItem::ItemState>();
    setProperty("Focus",metaEnum.key(state));

    style()->unpolish(this);
    style()->polish(this);
}
