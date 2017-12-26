#include "toolpage.h"

#include "head.h"
#include "datastruct.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QLabel>
#include <QStyle>
#include <QContextMenuEvent>
#include <QFontMetrics>
#include <QMenu>

#include "toolbox.h"

#define TOOL_SIMPLE_HEIGHT  24

class ToolPagePrivate : public GlobalData<ToolPage>
{
    Q_DECLARE_PUBLIC(ToolPage)
private:
    ToolPagePrivate(ToolPage * q):q_ptr(q)
    {
        initWidget();
        contextMenu = NULL;
    }

    void initWidget();

    void setToolName(QString text);

    void setExpanded(bool flag);

    ToolPage * q_ptr;
    ToolBox * toolBox;

    QWidget * contentWidget;
    QWidget * simpleTextWidget;
    QWidget * detailWidget;

    QLabel * iconLabel;
    QLabel * textLabel;
    QLabel * descLabel;

    QMenu * contextMenu;

    QList<ToolItem *> toolItems;

    bool expanded;
};

void ToolPagePrivate::initWidget()
{
    QHBoxLayout * mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    contentWidget = new QWidget;

    QVBoxLayout * contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    simpleTextWidget = new QWidget(contentWidget);
    simpleTextWidget->setObjectName("SimpleTextWidget");
    simpleTextWidget->setProperty("selected",false);
    simpleTextWidget->setFixedHeight(TOOL_SIMPLE_HEIGHT);

    detailWidget = new QWidget(contentWidget);

    contentLayout->addWidget(simpleTextWidget);
    contentLayout->addWidget(detailWidget);
    contentWidget->setLayout(contentLayout);

    QHBoxLayout * simpleLayout = new QHBoxLayout;
    simpleLayout->setContentsMargins(0,0,0,0);
    simpleLayout->setSpacing(0);

    iconLabel = new QLabel;
    iconLabel->setFixedSize(QSize(TOOL_SIMPLE_HEIGHT,TOOL_SIMPLE_HEIGHT));

    textLabel = new QLabel;
    textLabel->setFont(QFont("微软雅黑",13));
    textLabel->setObjectName("Tool_SimpleTextLabel");

    descLabel = new QLabel;
    textLabel->setObjectName("Tool_DescTextLabel");

    simpleLayout->addWidget(iconLabel);
    simpleLayout->addWidget(textLabel);
    simpleLayout->addWidget(descLabel);
    simpleTextWidget->setLayout(simpleLayout);

    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    setExpanded(false);
}

void ToolPagePrivate::setToolName(QString text)
{
    textLabel->setText(text);

    QFontMetrics fm(textLabel->font());
    textLabel->setFixedWidth(fm.width(textLabel->text()));
}

void ToolPagePrivate::setExpanded(bool flag)
{
    expanded = flag;
    detailWidget->setVisible(expanded);
    iconLabel->setStyleSheet("border-image:url("+
                             (expanded?QString(":/icon/resource/icon/Group_Expand_Tri.png")
                                     :QString(":/icon/resource/icon/Group_Close_Tri.png"))+")");

    if(!flag)
    {
        QList<ToolItem*>::iterator iter = toolItems.begin();
        while(iter != toolItems.end())
        {
            if((*iter)->isChecked())
            {
                (*iter)->setChecked(false);
            }

            iter++;
        }
    }
}


ToolPage::ToolPage(ToolBox *parent):
    d_ptr(new ToolPagePrivate(this)),
    QWidget(parent)
{
    d_ptr->toolBox = parent;
    d_ptr->detailWidget->installEventFilter(this);
    d_ptr->simpleTextWidget->installEventFilter(this);
}

ToolPage::~ToolPage()
{

}

/*!
     * @brief 设置工具页面内容
     *
     * @param[in] text 待设置内容
     *
     * @return 无
     *
     */
void ToolPage::setToolName(QString text)
{
    MQ_D(ToolPage);
    d->setToolName(text);
}

void ToolPage::addItem(ToolItem * item)
{
    MQ_D(ToolPage);

    if(!d->detailWidget->layout())
    {
        QVBoxLayout * layout = new QVBoxLayout;
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        d->detailWidget->setLayout(layout);
    }

    QVBoxLayout * layout = dynamic_cast<QVBoxLayout *>(d->detailWidget->layout());

    d->toolItems.append(item);

    layout->addWidget(item);
}

QList<ToolItem *>& ToolPage::items()
{
    MQ_D(ToolPage);
    return d->toolItems;
}

void ToolPage::setMenu(QMenu *menu)
{
    MQ_D(ToolPage);
    d->contextMenu = menu;
}

bool ToolPage::eventFilter(QObject *watched, QEvent *event)
{
    MQ_D(ToolPage);

    if(watched == d->simpleTextWidget)
    {
        if(event->type() == QEvent::Enter)
        {
            d->simpleTextWidget->setProperty("enter",true);
            style()->unpolish(d->simpleTextWidget);
            style()->polish(d->simpleTextWidget);
        }
        else if(event->type() == QEvent::Leave)
        {
            d->simpleTextWidget->setProperty("enter",false);
            style()->unpolish(d->simpleTextWidget);
            style()->polish(d->simpleTextWidget);
        }
        else if(event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent * e = dynamic_cast<QMouseEvent *>(event);
            if(e->button() == Qt::LeftButton)
            {
                d->setExpanded(!d->expanded);
            }
            else if(e->button() == Qt::RightButton)
            {
                emit selectedPage(this);
            }
        }
        else if(event->type() == QEvent::ContextMenu)
        {
            if(d->contextMenu)
            {
                d->contextMenu->exec(QCursor::pos());
            }

            return true;
        }
    }
    else if(watched == d->detailWidget)
    {
        if(event->type() == QEvent::ContextMenu)
        {
            return true;
        }
    }


    return QWidget::eventFilter(watched,event);
}
