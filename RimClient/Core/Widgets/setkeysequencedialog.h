/*!
 *  @brief     快捷键设置界面
 *  @details   设置软件的需要使用的快捷键
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.24
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef SETKEYSEQUENCEDIALOG_H
#define SETKEYSEQUENCEDIALOG_H

#include <QObject>
#include <QDialog>
class QVBoxLayout;
class QGroupBox;
class QLabel;
class QTableWidget;
class QHBoxLayout;
class QSpacerItem;
class QRadioButton;
class QMenuBar;
class QToolBar;
class QStatusBar;

class SetKeySequenceDialog : public QDialog
{
    Q_OBJECT
public:
    SetKeySequenceDialog(QWidget *parent);

private:
    void initWidget();

private:
    QVBoxLayout *verticalLayout_4;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QTableWidget *tblw_KeyList;
    QHBoxLayout *horizontalLayout;
    QPushButton *butt_ResetDefaultKey;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QVBoxLayout *verticalLayout;
    QRadioButton *rdbutt_Ctrl_Enter;
    QRadioButton *rdbutt_Enter;
};

#endif // SETKEYSEQUENCEDIALOG_H
