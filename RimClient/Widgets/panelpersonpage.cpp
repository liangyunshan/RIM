#include "panelpersonpage.h"

#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"

class PanelPersonPagePrivate : public GlobalData<PanelPersonPage>
{
    Q_DECLARE_PUBLIC(PanelPersonPage)

public:


};

PanelPersonPage::PanelPersonPage(QWidget *parent):
    d_ptr(new PanelPersonPagePrivate),
    QWidget(parent)
{
    initWidget();
}

PanelPersonPage::~PanelPersonPage()
{

}

void PanelPersonPage::initWidget()
{
    contentWidget = new QWidget(this);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    this->setLayout(mainLayout);
}
