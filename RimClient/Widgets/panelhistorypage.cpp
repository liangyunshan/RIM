#include "panelhistorypage.h"

#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"

class PanelHistoryPagePrivate : public GlobalData<PanelHistoryPage>
{
    Q_DECLARE_PUBLIC(PanelHistoryPage)

public:


};

PanelHistoryPage::PanelHistoryPage(QWidget *parent):
    d_ptr(new PanelHistoryPagePrivate),
    QWidget(parent)
{
    initWidget();
}

PanelHistoryPage::~PanelHistoryPage()
{

}

void PanelHistoryPage::initWidget()
{
    contentWidget = new QWidget(this);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    this->setLayout(mainLayout);

}
