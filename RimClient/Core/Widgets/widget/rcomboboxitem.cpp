#include "rcomboboxitem.h"

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include "rlabel.h"
#include "rsingleton.h"
#include "Util/imagemanager.h"

RComboBoxItem::RComboBoxItem(QWidget *parent):QWidget(parent)
{
    QWidget * widget = new QWidget(this);

    iconLabel = new RIconLabel;
    iconLabel->setTransparency(true);
    iconLabel->setEnterCursorChanged(false);
    iconLabel->setEnterHighlight(false);
    iconLabel->setCorner(true);

    iconLabel->setPixmap(RSingleton<ImageManager>::instance()->getSystemUserIcon());

    nickNameLabel = new QLabel(widget);
    nickNameLabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    nickNameLabel->setAlignment(Qt::AlignLeft);
    nickNameLabel->setMinimumWidth(125);
    nickNameLabel->setMinimumHeight(22);

    accountLabel = new QLabel(widget);
    accountLabel->setMinimumHeight(22);
    accountLabel->setAlignment(Qt::AlignLeft);

    closeButt = new QPushButton;
    closeButt->setText("X");
    closeButt->setFixedSize(20,20);
    closeButt->setToolTip(QObject::tr("Remove account"));
    connect(closeButt,SIGNAL(pressed()),this,SLOT(prepareDelete()));

    QGridLayout * gridlayout = new QGridLayout;
    gridlayout->setContentsMargins(2,2,2,2);
    gridlayout->addWidget(iconLabel,0,0,2,1);
    gridlayout->addWidget(nickNameLabel,0,1,1,6);
    gridlayout->addWidget(accountLabel,1,1,1,6);
    gridlayout->addWidget(closeButt,0,8,2,1);
    gridlayout->setHorizontalSpacing(1);
    gridlayout->setVerticalSpacing(1);

    widget->setLayout(gridlayout);

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(widget);
    setLayout(layout);
}

RComboBoxItem::~RComboBoxItem()
{

}

void RComboBoxItem::setNickName(QString name)
{
    nickNameLabel->setText(name);
}

void RComboBoxItem::setPixmap(QString pixmap)
{
    iconLabel->setPixmap(pixmap);
}

void RComboBoxItem::setAccountId(QString id)
{
    accountLabel->setText(id);
}

QString RComboBoxItem::getAccountId()
{
    return accountLabel->text();
}

void RComboBoxItem::mouseReleaseEvent(QMouseEvent *)
{
    emit itemClicked(accountLabel->text());
}

void RComboBoxItem::prepareDelete()
{
    emit deleteItem(accountLabel->text());
}
