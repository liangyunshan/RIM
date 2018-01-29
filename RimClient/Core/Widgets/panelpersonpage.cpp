#include "panelpersonpage.h"

#include <QHBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QLineEdit>

#include "head.h"
#include "datastruct.h"
#include "constants.h"
#include "maindialog.h"
#include "actionmanager/actionmanager.h"

#include "toolbox/toolbox.h"

using namespace GroupPerson;

class PanelPersonPagePrivate : public GlobalData<PanelPersonPage>
{
    Q_DECLARE_PUBLIC(PanelPersonPage)
protected:
    PanelPersonPagePrivate(PanelPersonPage * q):
        q_ptr(q)
    {
        initWidget();
    }

    void initWidget();

    PanelPersonPage * q_ptr;
    ToolBox * toolBox;

    QWidget * contentWidget;
    QLineEdit *tmpNameEdit;     //重命名时用edit
    ToolPage * pageOfMovedItem; //待移动分组的item所属的page

    QList<ToolPage *> pages;
    QList<ToolItem *> toolItems;
};

void PanelPersonPagePrivate::initWidget()
{
    contentWidget = new QWidget(q_ptr);
    contentWidget->setObjectName("Panel_Center_ContentWidget");

    QHBoxLayout * mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(contentWidget);
    q_ptr->setLayout(mainLayout);

    toolBox = new ToolBox(contentWidget);
    QObject::connect(toolBox,SIGNAL(updateGroupActions(ToolPage *)),q_ptr,SLOT(updateGroupActions(ToolPage *)));
    QHBoxLayout * contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(toolBox);
    contentWidget->setLayout(contentLayout);

    tmpNameEdit = new QLineEdit(toolBox);
    QObject::connect(tmpNameEdit,SIGNAL(editingFinished()),q_ptr,SLOT(renameEditFinished()));
    tmpNameEdit->setPlaceholderText(QObject::tr("untitled"));
    tmpNameEdit->hide();

    pageOfMovedItem = NULL;
}

PanelPersonPage::PanelPersonPage(QWidget *parent):
    d_ptr(new PanelPersonPagePrivate(this)),
    QWidget(parent)
{
    createAction();

    for(int j = 0; j < 5;j++)
    {
        ToolPage * page = d_ptr->toolBox->addPage(QStringLiteral("我的好友")+QString::number(j));
        d_ptr->pages.append(page);
        for(int i = 0; i < 5;i++)
        {
            ToolItem * item = new ToolItem(page);
            connect(item,SIGNAL(clearSelectionOthers(ToolItem*)),page,SIGNAL(clearItemSelection(ToolItem*)));
            connect(item,SIGNAL(showChatWindow(ToolItem*)),this,SLOT(createChatWindow(ToolItem*)));
            connect(item,SIGNAL(itemDoubleClick(ToolItem*)),MainDialog::instance(),SLOT(showChatWindow(ToolItem*)));
            connect(item,SIGNAL(itemMouseHover(bool,ToolItem*)),MainDialog::instance(),SLOT(showHoverItem(bool,ToolItem*)));
            connect(item,SIGNAL(updateGroupActions()),page,SLOT(updateGroupActions()));
            item->setContentMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM));
            item->setName(QString(QStringLiteral("韩天才%1")).arg(i));
            item->setNickName("MagnyCopper");
            item->setDescInfo(QStringLiteral("PSN确实快了很多啊！！！^_^"));
            page->addItem(item);
            d_ptr->toolItems.append(item);
        }

        page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
    }
}

PanelPersonPage::~PanelPersonPage()
{

}

/*!
     * @brief 刷新当前用户列表，重新发送请求至服务器
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelPersonPage::refreshList()
{

}

/*!
     * @brief 添加新分组
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelPersonPage::addGroup()
{
    MQ_D(PanelPersonPage);
    ToolPage * page = d->toolBox->addPage(QStringLiteral("untitled"));
    d->pages.append(page);
    page->setMenu(ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLGROUP));
    QRect textRec = d->toolBox->penultimatePage()->textRect();
    QRect pageRec = d->toolBox->penultimatePage()->geometry();
    int textY = pageRec.y()+page->txtFixedHeight();

    d->tmpNameEdit->raise();
    d->tmpNameEdit->setText(tr("untitled"));
    d->tmpNameEdit->selectAll();
    d->tmpNameEdit->setGeometry(textRec.x(),textY,pageRec.width(),page->txtFixedHeight());
    d->tmpNameEdit->show();
    d->tmpNameEdit->setFocus();
}

/*!
     * @brief 重命名分组
     *
     * @param[in] 无
     *
     * @return 无
     *
     */
void PanelPersonPage::renameGroup()
{
    MQ_D(PanelPersonPage);
    ToolPage * page = d->toolBox->selectedPage();
    if(page)
    {
        QRect textRec = page->textRect();
        QRect pageRec = page->geometry();
        d->tmpNameEdit->raise();
        d->tmpNameEdit->setText(page->toolName());
        d->tmpNameEdit->selectAll();
        d->tmpNameEdit->setGeometry(textRec.x(),pageRec.y(),pageRec.width(),textRec.height());
        d->tmpNameEdit->show();
        d->tmpNameEdit->setFocus();
    }
}

void PanelPersonPage::delGroup()
{

}

void PanelPersonPage::createChatWindow(ToolItem *item)
{
    Q_UNUSED(item);
}

void PanelPersonPage::sendInstantMessage()
{
     MQ_D(PanelPersonPage);
    createChatWindow(d->toolBox->selectedItem());
}

void PanelPersonPage::showUserDetail()
{

}

void PanelPersonPage::modifyUserInfo()
{

}

void PanelPersonPage::deleteUser()
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
void PanelPersonPage::renameEditFinished()
{
    MQ_D(PanelPersonPage);
    if(d->tmpNameEdit->text() != NULL)
    {
        d->toolBox->selectedPage()->setToolName(d->tmpNameEdit->text());
    }
    d->tmpNameEdit->setText("");
    d->tmpNameEdit->hide();
}

/*!
     * @brief 根据触发右键菜单的Item所属的Page来添加“移动联系人至”菜单中Action
     *
     * @param[in] page：ToolPage *
     *
     * @return 无
     *
     */
void PanelPersonPage::updateGroupActions(ToolPage * page)
{
    MQ_D(PanelPersonPage);
    d->pageOfMovedItem = page;
    QList <PersonGroupInfo> infoList = d->toolBox->toolPagesinfos();
    QMenu * movePersonTo  = ActionManager::instance()->menu(Constant::MENU_PANEL_PERSON_TOOLITEM_GROUPS);
    if(!movePersonTo->isEmpty())
    {
        movePersonTo->clear();
    }
    for(int index=0;index<infoList.count();index++)
    {
        if(page->pageInfo().uuid != infoList.at(index).uuid)
        {
            QAction *tmpAction = new QAction(infoList.at(index).name);
            tmpAction->setData(infoList.at(index).uuid);
            connect(tmpAction,SIGNAL(triggered(bool)),this,SLOT(movePersonTo()));
            movePersonTo->addAction(tmpAction);
        }
        else
        {
            continue;
        }
    }
}

/*!
     * @brief 移动至各分组的Action响应
     *
     * @param
     *
     * @return 无
     *
     */
void PanelPersonPage::movePersonTo()
{
    MQ_D(PanelPersonPage);
    QAction * target = qobject_cast<QAction *>(QObject::sender());
    QString targetUuid = target->data().toString();
    ToolPage * targetPage = d->toolBox->targetPage(targetUuid);
    ToolPage * sourcePage = d->pageOfMovedItem;
    ToolItem * targetItem = d->toolBox->selectedItem();
    if(!targetPage||!sourcePage)
    {
        return;
    }
    else
    {
        bool result = sourcePage->removeItem(targetItem);
        if(result)
        {
            targetPage->addItem(targetItem);
        }
    }
}

void PanelPersonPage::createAction()
{
    MQ_D(PanelPersonPage);

    //创建面板的右键菜单
    QMenu * menu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLBOX);

    QAction * freshAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_REFRESH,this,SLOT(refreshList()));
    freshAction->setText(tr("Refresh list"));

    QAction * addGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_ADDGROUP,this,SLOT(addGroup()));
    addGroupAction->setText(tr("Add group"));

    menu->addAction(freshAction);
    menu->addSeparator();
    menu->addAction(addGroupAction);

    d->toolBox->setContextMenu(menu);

    //创建分组的右键菜单
    QMenu * groupMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLGROUP);

    QAction * renameAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_RENAME,this,SLOT(renameGroup()));
    renameAction->setText(tr("Rename"));

    QAction * deleteGroupAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_PERSON_DELGROUP,this,SLOT(delGroup()));
    deleteGroupAction->setText(tr("Delete group"));

    groupMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_REFRESH));
    groupMenu->addSeparator();
    groupMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_PERSON_ADDGROUP));
    groupMenu->addAction(renameAction);
    groupMenu->addAction(deleteGroupAction);

    //创建联系人的右键菜单
    QMenu * personMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLITEM);

    QAction * sendMessAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_SENDMESSAGE,this,SLOT(sendInstantMessage()));
    sendMessAction->setText(tr("Send Instant Message"));

    QAction * viewDetailAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_VIEWDETAIL,this,SLOT(showUserDetail()));
    viewDetailAction->setText(tr("View Detail"));

    QAction * modifyAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_MODIFYCOMMENTS,this,SLOT(modifyUserInfo()));
    modifyAction->setText(tr("Modify Comments"));

    QAction * deletePersonAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_DELPERSON,this,SLOT(deleteUser()));
    deletePersonAction->setText(tr("Delete Friend"));

    QAction * movePersonTOAction = ActionManager::instance()->createAction(Constant::ACTION_PANEL_MOVEPERSON);
    movePersonTOAction->setText(tr("Move Friend To"));

    QMenu * groupsMenu = ActionManager::instance()->createMenu(Constant::MENU_PANEL_PERSON_TOOLITEM_GROUPS);
    movePersonTOAction->setMenu(groupsMenu);

    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_SENDMESSAGE));
    personMenu->addSeparator();
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_VIEWDETAIL));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MODIFYCOMMENTS));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_DELPERSON));
    personMenu->addAction(ActionManager::instance()->action(Constant::ACTION_PANEL_MOVEPERSON));
    personMenu->addSeparator();
}
