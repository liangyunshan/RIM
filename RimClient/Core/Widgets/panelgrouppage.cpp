#include "panelgrouppage.h"

#include <QLineEdit>
#include <QHBoxLayout>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "actionmanager/actionmanager.h"

#include "toolbox/toolbox.h"

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

    ToolBox * toolBox;
    QList<ToolPage *> pages;
    QList<ToolItem *> toolItems;
    QLineEdit *tmpNameEdit;     //重命名时用edit

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

    toolBox = new ToolBox(contentWidget);
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(toolBox);
    contentWidget->setLayout(contentLayout);

    tmpNameEdit = new QLineEdit(toolBox);
    QObject::connect(tmpNameEdit,SIGNAL(editingFinished()),q_ptr,SLOT(renameEditFinished()));
    tmpNameEdit->setPlaceholderText(QObject::tr("untitled"));
    tmpNameEdit->hide();

}

PanelGroupPage::PanelGroupPage(QWidget *parent) : QWidget(parent),
    d_ptr(new PanelGroupPagePrivate(this))
{
    createAction();

    ToolPage * page = d_ptr->toolBox->addPage(QStringLiteral("我的群"));
    d_ptr->pages.append(page);
    for(int i = 0; i < 5;i++)
    {
        ToolItem * item = new ToolItem(page);
        connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
        item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLITEM));
        item->setName(QStringLiteral("天地仁谷"));
        item->setNickName(QStringLiteral("(16人)"));
        page->addItem(item);
        d_ptr->toolItems.append(item);
    }

    page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_GROUP_TOOLGROUP));
}

PanelGroupPage::~PanelGroupPage()
{

}

void PanelGroupPage::searchGroup()
{

}

void PanelGroupPage::newGroup()
{

}

void PanelGroupPage::addGroups()
{

}

/*!
     * @brief 重命名分组
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelGroupPage::renameGroup()
{
    MQ_D(PanelGroupPage);
    ToolPage * page = d->toolBox->selectedPage();
    if(page)
    {
        QRect textRec = page->textRect();
        QRect pageRec = page->geometry();
        d->tmpNameEdit->raise();
        d->tmpNameEdit->setText(tr("untitled"));
        d->tmpNameEdit->selectAll();
        d->tmpNameEdit->setGeometry(textRec.x(),textRec.y(),pageRec.width(),textRec.height());
        d->tmpNameEdit->show();
        d->tmpNameEdit->setFocus();
    }
}

void PanelGroupPage::deleteGroup()
{

}

void PanelGroupPage::sendMessage()
{

}

void PanelGroupPage::viewDetail()
{

}

void PanelGroupPage::modifyGroupInfo()
{

}

void PanelGroupPage::exitGroup()
{

}

/*!
     * @brief LineEdit中命名完成后将内容设置为page的name
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelGroupPage::renameEditFinished()
{
    MQ_D(PanelGroupPage);
    if(d->tmpNameEdit->text() != NULL)
    {
        d->toolBox->selectedPage()->setToolName(d->tmpNameEdit->text());
    }
    d->tmpNameEdit->setText("");
    d->tmpNameEdit->hide();
}


void PanelGroupPage::createAction()
{
    MQ_D(PanelGroupPage);

    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLBOX);

    QAction * searchGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_SEARCHGROUP,this,SLOT(searchGroup()));
    searchGroupAction->setText(tr("Search group"));

    QAction * newGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_ADDGROUP,this,SLOT(newGroup()));
    newGroupAction->setText(tr("New group"));

    QAction * addGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_ADDGROUPS,this,SLOT(addGroups()));
    addGroupAction->setText(tr("Add groups"));

    menu->addAction(searchGroupAction);
    menu->addAction(newGroupAction);
    menu->addSeparator();
    menu->addAction(addGroupAction);

    d->toolBox->setContextMenu(menu);

    //创建分组的右键菜单
    QMenu * groupMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLGROUP);

    QAction * renameAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_RENAME,this,SLOT(renameGroup()));
    renameAction->setText(tr("Rename group"));

    QAction * delGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_DELGROUP,this,SLOT(deleteGroup()));
    delGroupAction->setText(tr("Delete group"));

    groupMenu->addAction(searchGroupAction);
    groupMenu->addAction(newGroupAction);
    groupMenu->addSeparator();
    groupMenu->addAction(addGroupAction);
    groupMenu->addAction(renameAction);
    groupMenu->addAction(delGroupAction);


    //创建item的右键菜单
    QMenu * itemMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_GROUP_TOOLITEM);

    QAction * sendMessAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_SENDMESS,this,SLOT(sendMessage()));
    sendMessAction->setText(tr("Send message"));

    QAction * viewDetailAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_VIEWDETAIL,this,SLOT(viewDetail()));
    viewDetailAction->setText(tr("View detail"));

    QAction * modifyAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_MODIFYCOMMENTS,this,SLOT(modifyGroupInfo()));
    modifyAction->setText(tr("Modify name"));

    QAction * exitGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_GROUP_EXITGROUP,this,SLOT(exitGroup()));
    exitGroupAction->setText(tr("Exit group"));

    itemMenu->addAction(sendMessAction);
    itemMenu->addSeparator();
    itemMenu->addAction(viewDetailAction);
    itemMenu->addSeparator();
    itemMenu->addAction(modifyAction);
    itemMenu->addSeparator();
    itemMenu->addAction(exitGroupAction);
}
