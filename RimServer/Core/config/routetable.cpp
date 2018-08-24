#include "routetable.h"
#include "ui_routetable.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QWidget>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

#include "routetableservermodel.h"
#include "routetableclientmodel.h"
#include "routetableview.h"
#include "../protocol/datastruct.h"
#include "routetableclientdelegate.h"
#include "routetableserverdelegate.h"
#include "routesettings.h"
#include "../global.h"
#include "../actionmanager/actionmanager.h"
#include "Util/rsingleton.h"
#include "../constants.h"
#include "routetablehead.h"
#include "../file/xmlparse.h"

RouteTable::RouteTable(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RouteTable),currRouteSettings(NULL)
{
    ui->setupUi(this);

    setWindowTitle(tr("Route table"));

    Qt::WindowFlags  flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    initContextMenu();
    initView();

    connect(ui->confirm,SIGNAL(pressed()),this,SLOT(saveFile()));
    connect(ui->cancel,SIGNAL(pressed()),this,SLOT(cancelSave()));
}

RouteTable::~RouteTable()
{
    if(currRouteSettings)
        delete currRouteSettings;
    delete ui;
}

void RouteTable::initView()
{
    currRouteSettings = new RouteSettings;
    currRouteSettings->initSettings(RGlobal::G_RouteSettings);

    //Server
    QWidget * serverWidget = new QWidget(ui->widget_3);

    QLabel * serverLabel = new QLabel(serverWidget);
    serverLabel->setText(tr("Server node:"));

    RouteTableServerDelegate * serverDelegate = new RouteTableServerDelegate;
    serverModel = new RouteTableServerModel;
    serverModel->setRouteData(currRouteSettings);
    serverView = new RouteTableView(V_SERVER);
    serverView->horizontalHeader()->resizeSection(0,120);
    serverView->horizontalHeader()->resizeSection(1,150);

    serverView->setItemDelegate(serverDelegate);
    serverView->setModel(serverModel);

    QVBoxLayout * serverLayout = new QVBoxLayout;
    serverLayout->setContentsMargins(1,1,1,1);
    serverLayout->setSpacing(4);
    serverLayout->addWidget(serverLabel);
    serverLayout->addWidget(serverView);

    serverWidget->setLayout(serverLayout);

    //Client
    QWidget * clientWidget = new QWidget(ui->widget_3);

    QLabel * clientLabel = new QLabel(clientWidget);
    clientLabel->setText(tr("Client node:"));

    RouteTableClientDelegate * clientDelegate = new RouteTableClientDelegate;
    clientModel = new RouteTableClientModel;
    clientModel->setRouteData(currRouteSettings);
    clientView = new RouteTableView(V_CLIENT);

    clientView->setItemDelegate(clientDelegate);
    clientView->setModel(clientModel);

    QVBoxLayout * clientLayout = new QVBoxLayout;
    clientLayout->setContentsMargins(1,1,1,1);
    clientLayout->setSpacing(4);
    clientLayout->addWidget(clientLabel);
    clientLayout->addWidget(clientView);

    clientWidget->setLayout(clientLayout);

    QSplitter * verticalSplitter = new QSplitter(Qt::Vertical);
    verticalSplitter->addWidget(serverWidget);
    verticalSplitter->addWidget(clientWidget);

    connect(serverView,SIGNAL(clicked(QModelIndex)),this,SLOT(switchServerIndex(QModelIndex)));

    QHBoxLayout * layout = new QHBoxLayout;
    layout->setContentsMargins(4,4,4,4);
    layout->addWidget(verticalSplitter);
    ui->widget_3->setLayout(layout);

    ui->cancel->setText(tr("Cancel"));
    ui->confirm->setText(tr("Save"));
}

void RouteTable::initContextMenu()
{
    QMenu * serverMenu = ActionManager::instance()->createMenu(Constant::MENU_ROUTE_TABLE_SERVER);
    QAction * serverNew = ActionManager::instance()->createAction(Constant::ACTION_ROUTE_SERVER_NEW,this,SLOT(addNewServerGroup(bool)));
    serverNew->setText(tr("New"));
    QAction * serverDelete = ActionManager::instance()->createAction(Constant::ACTION_ROUTE_SERVER_DELETE,this,SLOT(deleteServerGroup(bool)));
    serverDelete->setText(tr("Delete"));
    serverMenu->addAction(serverNew);
    serverMenu->addAction(serverDelete);

    QMenu * clientMenu = ActionManager::instance()->createMenu(Constant::MENU_ROUTE_TABLE_CLIENT);
    QAction * clientNew = ActionManager::instance()->createAction(Constant::ACTION_ROUTE_CLIENT_NEW,this,SLOT(addNewClientGroup(bool)));
    clientNew->setText(tr("New"));
    QAction * clientMove = ActionManager::instance()->createAction(Constant::ACTION_ROUTE_CLIENT_MOVE,this,SLOT(moveClientGroup(bool)));
    clientMove->setText(tr("Move"));
    QAction * clientDelete = ActionManager::instance()->createAction(Constant::ACTION_ROUTE_CLIENT_DELETE,this,SLOT(deleteClientGroup(bool)));
    clientDelete->setText(tr("Delete"));
    clientMenu->addAction(clientNew);
    clientMenu->addAction(clientDelete);
}

void RouteTable::switchServerIndex(QModelIndex index)
{
    ParameterSettings::NodeServer * server = static_cast<ParameterSettings::NodeServer *>(index.internalPointer());
    if(server){
       clientModel->setCurrServer(server->nodeId);
    }
}

/*!
 * @brief  服务器端添加新的分组
 */
void RouteTable::addNewServerGroup(bool)
{

}

/*!
 * @brief  服务器端添删除分组
 */
void RouteTable::deleteServerGroup(bool)
{

}

/*!
 * @brief  客户端添加分组
 */
void RouteTable::addNewClientGroup(bool)
{

}

/*!
 * @brief  移动客户端分组至其它服务器节点
 */
void RouteTable::moveClientGroup(bool)
{

}

/*!
 * @brief  客户端删除分组
 */
void RouteTable::deleteClientGroup(bool)
{

}

/*!
 * @brief 将当前信息保存更新路由表
 */
void RouteTable::saveFile()
{
    bool flag = RSingleton<XMLParse>::instance()->saveRouteSettings(qApp->property(Constant::LOCAL_ROUTE_CONFIG_FILE).toString(),currRouteSettings->settings());
    if(flag){
        QMessageBox::information(this,tr("information"),tr("Save successfully! Modifications will take effect after reboot."),QMessageBox::Yes);
    }else{
        QMessageBox::information(this,tr("information"),tr("Save failed!"),QMessageBox::Yes);
    }
}

void RouteTable::cancelSave()
{
    int result = QMessageBox::information(this,tr("inforamtion"),tr("This action will discard the modification,Whether to continue!"),QMessageBox::Yes|QMessageBox::No,QMessageBox::No);
    if(result == QMessageBox::No)
        return;
    close();
}
