#include "paneltoparea.h"

#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>

#include "datastruct.h"
#include "constants.h"
#include "head.h"
#include "Util/imagemanager.h"
#include "Util/rsingleton.h"

class PanelTopAreaPrivate : public GlobalData<PanelTopArea>
{
    Q_DECLARE_PUBLIC(PanelTopArea)

private:


};

PanelTopArea::PanelTopArea(QWidget *parent) :
    d_ptr(new PanelTopAreaPrivate()),
    QWidget(parent)
{
    initWidget();
}

PanelTopArea::~PanelTopArea()
{

}

#define PANEL_TOP_USER_ICON_SIZE 66
#define PANEL_TOP_SEARCH_ICON_SIZE 28

void PanelTopArea::initWidget()
{
    contentWidget = new QWidget(this);
    contentWidget->setObjectName("Panel_Top_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(1);
    mainLayout->addWidget(contentWidget);
    this->setLayout(mainLayout);

    QGridLayout * gridLayout = new QGridLayout(contentWidget);

    QWidget * iconWidget = new QWidget(contentWidget);
    iconWidget->setFixedSize(PANEL_TOP_USER_ICON_SIZE + 15,PANEL_TOP_USER_ICON_SIZE);
    QHBoxLayout * iconLayout = new QHBoxLayout;
    iconLayout->setContentsMargins(0,0,0,0);

    userIconLabel = new QLabel(contentWidget);
    userIconLabel->setObjectName("Panel_Top_UserIconLabel");
    userIconLabel->setFixedSize(PANEL_TOP_USER_ICON_SIZE,PANEL_TOP_USER_ICON_SIZE);
    userIconLabel->setStyleSheet(QString("border-image:url(%1);").arg(RSingleton<ImageManager>::instance()->getSystemUserIcon()));

    iconLayout->addStretch(1);
    iconLayout->addWidget(userIconLabel);
    iconWidget->setLayout(iconLayout);

    userNikcNameLabel = new QLabel(contentWidget);
    userNikcNameLabel->setObjectName("Panel_Top_UserNikcNameLabel");
    userNikcNameLabel->setFixedHeight(PANEL_TOP_USER_ICON_SIZE / 3);
    userNikcNameLabel->setText("一路向北");

    userSignNameEdit = new QLineEdit(contentWidget);
    userSignNameEdit->setObjectName("Panel_Top_UserSignNameEdit");
    userSignNameEdit->setFixedHeight(PANEL_TOP_USER_ICON_SIZE / 3);
    userSignNameEdit->setText("Say Something");

    extendToolWiget = new QWidget(contentWidget);
    extendToolWiget->setFixedHeight(PANEL_TOP_USER_ICON_SIZE / 3);

    QWidget * searchWidget = new QWidget(contentWidget);
    searchWidget->setObjectName("Panel_Top_SearchWidget");
    QHBoxLayout * searchLayout = new QHBoxLayout();
    searchLayout->setContentsMargins(5,0,0,0);
    searchLayout->setSpacing(1);

    QLabel * iconLabel = new QLabel(searchWidget);
    iconLabel->setPixmap(QPixmap(Constant::ICON_PANEL_SEARCH));
    iconLabel->setFixedSize(PANEL_TOP_SEARCH_ICON_SIZE,PANEL_TOP_SEARCH_ICON_SIZE);
    iconLabel->setToolTip(tr("Search person/group"));

    searchLineEdit = new QLineEdit(contentWidget);
    searchLineEdit->setFixedHeight(PANEL_TOP_SEARCH_ICON_SIZE);
    searchLineEdit->setObjectName("Panel_Top_SearchLineEdit");
    searchLineEdit->setPlaceholderText(tr("Search"));

    searchLayout->addWidget(iconLabel);
    searchLayout->addWidget(searchLineEdit);
    searchWidget->setLayout(searchLayout);

    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setRowStretch(0,4);
    gridLayout->setVerticalSpacing(5);
    gridLayout->setHorizontalSpacing(5);

    gridLayout->addWidget(iconWidget,1,0,3,1);
    gridLayout->addWidget(userNikcNameLabel,1,1,1,1);
    gridLayout->addWidget(userSignNameEdit,2,1,1,1);
    gridLayout->addWidget(extendToolWiget,3,1,1,1);
    gridLayout->setRowStretch(4,1);
    gridLayout->addWidget(searchWidget,5,0,1,2);

    contentWidget->setLayout(gridLayout);

}
