#include "setkeysequencedialog.h"
#include "global.h"
#include "constants.h"
#include "actionmanager/shortcutsettings.h"
#include "rsingleton.h"
#include "screenshot.h"

#include <QtCore/QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QStatusBar>
#include <QTableWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include <QDebug>
#include <QKeySequence>
#include <QClipboard>
#include <QMimeData>
#include <QProcess>
#include <QTimer>
#include <QKeyEvent>

SetKeySequenceDialog::SetKeySequenceDialog(QWidget *parent):
    QDialog(parent)
{
    setModal(true);
    initWidget();
    loadSetting();
}

SetKeySequenceDialog::~SetKeySequenceDialog()
{
    qDebug()<<__FILE__<<__LINE__<<"\n"
           <<""<<this
          <<"\n";
}

void SetKeySequenceDialog::initWidget()
{
    SetKeySequenceDialog *Dialog = this;
    if (Dialog->objectName().isEmpty())
        Dialog->setObjectName(QString::fromUtf8("SetKeySequenceDialog"));
    Dialog->resize(500, 450);
    verticalLayout_4 = new QVBoxLayout(Dialog);
    verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
    groupBox_2 = new QGroupBox(Dialog);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
    verticalLayout_3 = new QVBoxLayout(groupBox_2);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    label_3 = new QLabel(groupBox_2);
    label_3->setObjectName(QString::fromUtf8("label_3"));

    verticalLayout_3->addWidget(label_3);

    tblw_KeyList = new QTableWidget(groupBox_2);
    tblw_KeyList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (tblw_KeyList->columnCount() < 3)
        tblw_KeyList->setColumnCount(3);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    tblw_KeyList->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    tblw_KeyList->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    tblw_KeyList->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    tblw_KeyList->setObjectName(QString::fromUtf8("tblw_KeyList"));
    tblw_KeyList->installEventFilter(this);

    verticalLayout_3->addWidget(tblw_KeyList);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    butt_ResetDefaultKey = new QPushButton(groupBox_2);
    butt_ResetDefaultKey->setObjectName(QString::fromUtf8("butt_ResetDefaultKey"));

    horizontalLayout->addWidget(butt_ResetDefaultKey);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);


    verticalLayout_3->addLayout(horizontalLayout);


    verticalLayout_4->addWidget(groupBox_2);

    groupBox = new QGroupBox(Dialog);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    verticalLayout_2 = new QVBoxLayout(groupBox);
    verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
    label_4 = new QLabel(groupBox);
    label_4->setObjectName(QString::fromUtf8("label_4"));

    verticalLayout_2->addWidget(label_4);

    verticalLayout = new QVBoxLayout();
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    rdbutt_Ctrl_Enter = new QRadioButton(groupBox);
    rdbutt_Ctrl_Enter->setObjectName(QString::fromUtf8("rdbutt_Ctrl_Enter"));
    rdbutt_Ctrl_Enter->setChecked(false);
    connect(rdbutt_Ctrl_Enter,SIGNAL(toggled(bool)),this,SLOT(slot_RadioButton_CtrlEnter(bool)));

    verticalLayout->addWidget(rdbutt_Ctrl_Enter);

    rdbutt_Enter = new QRadioButton(groupBox);
    rdbutt_Enter->setObjectName(QString::fromUtf8("rdbutt_Enter"));
    rdbutt_Enter->setChecked(true);
    connect(rdbutt_Enter,SIGNAL(toggled(bool)),this,SLOT(slot_RadioButton_Enter(bool)));

    verticalLayout->addWidget(rdbutt_Enter);

    verticalLayout_2->addLayout(verticalLayout);

    verticalLayout_4->addWidget(groupBox);

    //tr翻译
    this->setWindowTitle(tr("SetKeySequenceDialog"));           //设置全局快捷键
    groupBox_2->setTitle(tr("set global KeySequence"));         //全局热键
    label_3->setText(tr("can click to change hotkey"));         //你可以通过点击选择需要修改的热键
    QTableWidgetItem *___qtablewidgetitem = tblw_KeyList->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(tr("function describ"));       //功能描述
    QTableWidgetItem *___qtablewidgetitem1 = tblw_KeyList->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(tr("hot key"));               //热键
    QTableWidgetItem *___qtablewidgetitem2 = tblw_KeyList->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(tr("KeySequence status"));    //状态
    butt_ResetDefaultKey->setText(tr("ResetDefaultKey"));       //恢复默认热键
    groupBox->setTitle(tr("quik hotkey set"));                  //加速键
    label_4->setText(tr("chat dialog send msg"));               //会话窗口中发送消息
    rdbutt_Ctrl_Enter->setText(tr("rdbutt_Ctrl_Enter"));        //按Ctrl + Enter键
    rdbutt_Enter->setText(tr("rdbutt_Enter"));                  //按Enter键

    appendTableWidgetItem(tblw_KeyList,2);
}

//读取并显示配置信息
void SetKeySequenceDialog::loadSetting()
{
    QKeySequence Button_Chat_Close_Window = RSingleton<ShortcutSettings>::instance()->shortCut(Constant::Button_Chat_Close_Window);
    QKeySequence key_Button_Chat_Shot = RSingleton<ShortcutSettings>::instance()->shortCut(Constant::ACTION_CHAT_SCREEN_SHOT);
    QKeySequence key_Button_Chat_Send = RSingleton<ShortcutSettings>::instance()->shortCut(Constant::Button_Chat_Send);

    QString status_normal = tr("normal");   //热键正常
    QString status_error = tr("error");     //热键失常

    //关闭窗口快捷键
    tblw_KeyList->item(0,0)->setText(tr("Chat_Close_Window"));
    tblw_KeyList->item(0,1)->setText(Button_Chat_Close_Window.toString());
    if(Button_Chat_Close_Window.isDetached())
    {
        tblw_KeyList->item(0,2)->setText(status_normal);
    }
    else
    {
        tblw_KeyList->item(0,2)->setText(status_error);
    }

    //截屏快捷键
    tblw_KeyList->item(1,0)->setText(tr("key_Button_Chat_Shot"));
    tblw_KeyList->item(1,1)->setText(key_Button_Chat_Shot.toString());
    if(key_Button_Chat_Shot.isDetached())
    {
        tblw_KeyList->item(1,2)->setText(status_normal);
    }
    else
    {
        tblw_KeyList->item(1,2)->setText(status_error);
    }

    updateSenMsgRadioButton(key_Button_Chat_Send.toString());
}

void SetKeySequenceDialog::slot_RadioButton_Enter(bool)
{
    setSendMsgShortCut(Qt::Key_Return);
}

void SetKeySequenceDialog::slot_RadioButton_CtrlEnter(bool)
{
    setSendMsgShortCut(Qt::Key_Control+Qt::Key_Return);
}

void SetKeySequenceDialog::keyPressEvent(QKeyEvent *event)
{
    if(event->key())
    {
        m_tempKeyString = "";

        QKeySequence key = QKeySequence(event->modifiers() + event->key());
        if(event->key()>=Qt::Key_A && event->key()<=Qt::Key_Z)
        {
            if(RSingleton<ShortcutSettings>::instance()->contains(key))
            {
                QDialog::keyPressEvent(event);
                return ;
            }
            QTableWidgetItem *item_0 = tblw_KeyList->item(0,1);
            QTableWidgetItem *item_1 = tblw_KeyList->item(1,1);
            QTableWidgetItem *item_2 = tblw_KeyList->item(2,1);

            QTableWidgetItem *item_selceted=NULL;
            if(item_0->isSelected())
            {
                item_selceted = item_0;
            }
            else if(item_1->isSelected())
            {
                item_selceted = item_1;
                RSingleton<ScreenShot>::instance()->setScreenShotKey(key);
            }
            else if(item_2->isSelected())
            {
                item_selceted = item_2;
            }
            else
            {
               QDialog::keyPressEvent(event);
               return ;
            }
            m_tempKeyString = key.toString();
            item_selceted->setText(m_tempKeyString);
        }
    }
    QDialog::keyPressEvent(event);
}

void SetKeySequenceDialog::setSendMsgShortCut(int key)
{
    G_mIsEnter = key;
    if(G_mIsEnter == Qt::Key_Return)
    {
        RSingleton<ShortcutSettings>::instance()->removeShortCut(Constant::Button_Chat_Send);
        RSingleton<ShortcutSettings>::instance()->addShortcut(Id(Constant::Button_Chat_Send),QKeySequence("Enter"));
        RSingleton<ShortcutSettings>::instance()->save();
    }
    else if(G_mIsEnter == (Qt::Key_Control+Qt::Key_Return))
    {
        RSingleton<ShortcutSettings>::instance()->removeShortCut(Constant::Button_Chat_Send);
        RSingleton<ShortcutSettings>::instance()->addShortcut(Id(Constant::Button_Chat_Send),QKeySequence("Ctrl+Enter"));
        RSingleton<ShortcutSettings>::instance()->save();
    }
}

void SetKeySequenceDialog::updateSenMsgRadioButton(QString key)
{
    if(QKeySequence(key).toString() == QKeySequence("Enter").toString())
    {
        G_mIsEnter = Qt::Key_Return;
        rdbutt_Enter->setChecked(true);
        rdbutt_Ctrl_Enter->setChecked(false);
    }
    else
    {
        G_mIsEnter = Qt::Key_Control + Qt::Key_Return;
        rdbutt_Enter->setChecked(false);
        rdbutt_Ctrl_Enter->setChecked(true);
    }
}

//添加一行表格条目
void SetKeySequenceDialog::appendTableWidgetItem(QTableWidget *table,int nums)
{
    if(table==NULL)
    {
        return ;
    }
    int startindex = 0;
    if(nums>0)
    {
        int rowcount = table->rowCount();
        if(rowcount>0)
        {
            startindex = rowcount-1;
            table->setRowCount(startindex+nums+1);
        }
        else
        {
            table->setRowCount(startindex+nums);
        }
    }
    for(int i=startindex;i<(startindex+nums);i++)
    {
        for(int column=0;column<table->columnCount();column++)
        {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setText((""));
            item->setTextAlignment(Qt::AlignCenter);
            if(column!=1)
            {
                item->setFlags(Qt::ItemIsEnabled);
            }
            table->setItem(i,column,item);
        }
    }
}

