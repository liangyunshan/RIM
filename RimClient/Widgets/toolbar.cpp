#include "toolbar.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QStyle>
#include <QDebug>

#include "constants.h"
#include "actionmanager/actionmanager.h"

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
    toolBarLayout->addStretch(1);
    toolBar->setLayout(toolBarLayout);

    content = new QWidget(this);

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(1);
    layout->addWidget(toolBar);
    layout->addWidget(content);

    this->setLayout(layout);

    RToolButton * minSize = ActionManager::instance()->createToolButton(Id(Constant::TOOL_MIN),this,SIGNAL(minimumWindow()));
    minSize->setToolTip(tr("Min"));
//    minSize->setIcon(style()->standardPixmap(QStyle::SP_TitleBarMinButton));

    RToolButton * closeButt = ActionManager::instance()->createToolButton(Id(Constant::TOOL_CLOSE),this,SIGNAL(closeWindow()));
    closeButt->setToolTip(tr("Close"));
//    closeButt->setIcon(style()->standardPixmap(QStyle::SP_TitleBarCloseButton));

    appendToolButton(minSize);
    appendToolButton(closeButt);
}


bool ToolBar::appendToolButton(RToolButton *toolButton)
{
    if(!containButton(toolButton))
    {
        toolButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
        toolBarLayout->addWidget(toolButton);
    }
    return true;
}

bool ToolBar::insertToolButton(RToolButton *toolButton, const char *ID)
{
    if(!containButton(toolButton))
    {
        if(ID == NULL)
        {
            toolBarLayout->insertWidget(0,toolButton);
            return true;
        }
        else
        {
            int index = 0;
            foreach(QObject * tmpObj,toolBar->children())
            {
                RToolButton * tmpButt = dynamic_cast<RToolButton *>(tmpObj);
                if(tmpButt && tmpButt->objectName() == QString(ID))
                {
                    toolButton->setFixedSize(Constant::TOOL_WIDTH,Constant::TOOL_HEIGHT);
                    toolBarLayout->insertWidget(index,toolButton);
                    return true;
                }
                index++;
            }
        }

        return false;
    }
    return true;
}

bool ToolBar::containButton(RToolButton *button)
{
    foreach(RToolButton * tmpButton,toolButts)
    {
        if(button->objectName() == tmpButton->objectName())
        {
            return true;
        }
    }
    return false;
}
