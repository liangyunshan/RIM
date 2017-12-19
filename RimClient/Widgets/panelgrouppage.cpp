#include "panelgrouppage.h"

#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"

class PanelGroupPagePrivate : public GlobalData<PanelGroupPage>
{
    Q_DECLARE_PUBLIC(PanelGroupPage)

public:
    PanelGroupPagePrivate(PanelGroupPage * q):
        q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    PanelGroupPage * q_ptr;

    QWidget * contentWidget;
};

void PanelGroupPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);
}

PanelGroupPage::PanelGroupPage(QWidget *parent) : QWidget(parent),
    d_ptr(new PanelGroupPagePrivate(this))
{

}

PanelGroupPage::~PanelGroupPage()
{

}
