#include "chattextframe.h"
#include <QTextEdit>

ChatTextFrame::ChatTextFrame(QTextEdit *parent) :
    p_parentTextEdit(parent)
{
    p_currTextFrameFormat = new QTextFrameFormat();
}

ChatTextFrame::~ChatTextFrame()
{
    if(p_currTextFrameFormat)
    {
        delete p_currTextFrameFormat;
        p_currTextFrameFormat = NULL;
    }
}

QTextFrameFormat *ChatTextFrame::currTextFrameFormat()
{
    return p_currTextFrameFormat;
}
