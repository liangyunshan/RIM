#include "panelhistorypage.h"

#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"

class PanelHistoryPagePrivate : public GlobalData<PanelHistoryPage>
{
    Q_DECLARE_PUBLIC(PanelHistoryPage)

public:
    PanelHistoryPagePrivate(PanelHistoryPage * q):
        q_ptr(q)
    {
        initWidget();
    }
    void initWidget();

    PanelHistoryPage * q_ptr;
     QWidget * contentWidget;
};

void PanelHistoryPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);
}

PanelHistoryPage::PanelHistoryPage(QWidget *parent):
    d_ptr(new PanelHistoryPagePrivate(this)),
    QWidget(parent)
{

}

PanelHistoryPage::~PanelHistoryPage()
{

}
