#include "toolbar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QSpacerItem>
#include <QStyle>
#include <QDebug>

#include "constants.h"

ToolBar::ToolBar(QWidget *parent) :
    QWidget(parent)
{
    initDialog();
}

ToolBar::~ToolBar()
{

}

void ToolBar::initDialog()
{
    toolBar = new QWidget(this);

    toolBarLayout = new QHBoxLayout();

    toolBarLayout->setContentsMargins(0,0,0,0);
    toolBarLayout->setSpacing(0);
    toolBar->setLayout(toolBarLayout);

    content = new QWidget(this);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    layout->addWidget(toolBar);
    layout->addWidget(content);

    this->setLayout(layout);

    minSize = new QToolButton(this);
    minSize->setObjectName(Constant::TOOL_MIN);
    minSize->setToolTip(tr("Min"));
//    minSize->setIcon(style()->standardPixmap(QStyle::SP_TitleBarMinButton));
    connect(minSize,SIGNAL(pressed()),this,SIGNAL(minimumWindow()));

    closeButt = new QToolButton(this);
    closeButt->setObjectName(Constant::TOOL_CLOSE);
    closeButt->setToolTip(tr("Close"));
//    closeButt->setIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton));
    connect(closeButt,SIGNAL(pressed()),this,SIGNAL(closeWindow()));

    insertToolButton(minSize);
    insertToolButton(closeButt);
}


bool ToolBar::insertToolButton(QToolButton *toolButton)
{
    if(!containButton(toolButton))
    {
        toolButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
        toolBarLayout->addWidget(toolButton);
    }
    return true;
}

bool ToolBar::insertToolButton(QToolButton *toolButton, const char *ID)
{
    if(!containButton(toolButton))
    {
        int index = -1;
        foreach(QObject * tmpObj,toolBar->children())
        {
            QToolButton * tmpButt = dynamic_cast<QToolButton *>(tmpObj);
            if(tmpButt && tmpButt->objectName() == QString(ID))
            {
                toolButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
                toolBarLayout->insertWidget(index,toolButton);
                return true;
            }
            index++;
        }

        return false;
    }
    return true;
}

bool ToolBar::containButton(QToolButton *button)
{
    foreach(QToolButton * tmpButton,toolButts)
    {
        if(button->objectName() == tmpButton->objectName())
        {
            return true;
        }
    }
    return false;
}
