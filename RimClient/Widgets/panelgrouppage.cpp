#include "panelgrouppage.h"

#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"

class PanelGroupPagePrivate : public GlobalData<PanelGroupPage>
{
    Q_DECLARE_PUBLIC(PanelGroupPage)

public:


};

PanelGroupPage::PanelGroupPage(QWidget *parent) : QWidget(parent),
    d_ptr(new PanelGroupPagePrivate)
{
    initWidget();
}

PanelGroupPage::~PanelGroupPage()
{

}

void PanelGroupPage::initWidget()
{
    contentWidget = new QWidget(this);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    this->setLayout(mainLayout);
}
