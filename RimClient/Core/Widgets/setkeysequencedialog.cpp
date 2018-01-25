#include "setkeysequencedialog.h"
#include "global.h"

#include <QDebug>
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

SetKeySequenceDialog::SetKeySequenceDialog(QWidget *parent):
    QDialog(parent)
{
    setModal(true);
    initWidget();
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
    if (tblw_KeyList->columnCount() < 3)
        tblw_KeyList->setColumnCount(3);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    tblw_KeyList->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    tblw_KeyList->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    tblw_KeyList->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    tblw_KeyList->setObjectName(QString::fromUtf8("tblw_KeyList"));

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

    verticalLayout->addWidget(rdbutt_Ctrl_Enter);

    rdbutt_Enter = new QRadioButton(groupBox);
    rdbutt_Enter->setObjectName(QString::fromUtf8("rdbutt_Enter"));
    rdbutt_Enter->setChecked(true);

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

}
