#include "panelcontentpage.h"

#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"
#include "constants.h"

class PanelContentPagePrivate : public GlobalData<PanelContentPage>
{
    Q_DECLARE_PUBLIC(PanelContentPage)

private:
    QWidget * contentWidget;

};

PanelContentPage::PanelContentPage(QWidget *parent) : QWidget(parent),
    d_ptr(new PanelContentPagePrivate())
{
    initWidget();
}

PanelContentPage::~PanelContentPage()
{

}

void PanelContentPage::setContentWidget(QWidget *wid)
{
    MQ_D(PanelContentPage);

    QHBoxLayout * layout = NULL;

    if(d->contentWidget->children().size() == 0)
    {
        QHBoxLayout * contentLayout = new QHBoxLayout;
        contentLayout->setContentsMargins(0,0,0,0);
        contentLayout->setSpacing(0);
        d->contentWidget->setLayout(contentLayout);

        layout = contentLayout;
    }
    else
    {
        layout = dynamic_cast<QHBoxLayout *>(d->contentWidget->children().at(0));
    }

    if(layout)
    {
        layout->addWidget(wid);
    }
}

void PanelContentPage::initWidget()
{
    MQ_D(PanelContentPage);

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    d->contentWidget = new QWidget();

    mainLayout->addWidget(d->contentWidget);
    setLayout(mainLayout);

}
