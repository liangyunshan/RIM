/*!
 *  @brief     自定义提示窗
 *  @details   自定义提示窗口，主要实现窗口样式统一
 *  @file      rmessagebox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.02
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note      20180102:wey:修复窗口退出程序奔溃(取消Qt::WA_DeleteOnClose设置)
 */
#ifndef RMESSAGEBOX_H
#define RMESSAGEBOX_H

#include <QDialog>

class RMessageBoxPrivate;
class RButton;

class RMessageBox : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RMessageBox)
public:
    RMessageBox(QWidget * parent = 0);
    ~RMessageBox();

    void setWindowTitle(const QString & text);
    QString windowTitle()const;

    void setText(const QString & text);
    QString text()const;

    enum Icon
    {
        NoIcon = 0,
        Information = 1,
        Warning = 2,
        Critical = 3,
        Question = 4
    };
    Q_ENUM(Icon)

    void setIcon(RMessageBox::Icon icon);

    enum StandardButton
    {
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,

        FirstButton        = Ok,                // internal
        LastButton         = RestoreDefaults,   // internal

        YesAll             = YesToAll,          // obsolete
        NoAll              = NoToAll,           // obsolete

        Default            = 0x00000100,        // obsolete
        Escape             = 0x00000200,        // obsolete
        FlagMask           = 0x00000300,        // obsolete
        ButtonMask         = ~FlagMask          // obsolete
    };
    Q_DECLARE_FLAGS(StandardButtons, StandardButton)
    Q_FLAGS(StandardButtons)

    static int information(QWidget * parent, const QString &title, const QString& text, int butts, StandardButton defaultButton = NoButton);
    static int warning(QWidget * parent, const QString &title, const QString& text, int butts, StandardButton = NoButton);

    RMessageBox::StandardButton clickedButton()const;

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

    void closeEvent(QCloseEvent * event);

private slots:
    void respButtonClicked();

private:
    RButton * addButton(RMessageBox::StandardButton butt);
    QString standardButtText(RMessageBox::StandardButton butt);
    static int messagebox(QWidget * parent, Icon type, const QString &title, const QString& text, int butts, StandardButton = NoButton);

private:
    RMessageBoxPrivate * d_ptr;

};

#endif // RMESSAGEBOX_H
