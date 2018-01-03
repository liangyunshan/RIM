/*!
 *  @brief     自定义提示窗
 *  @details   自定义提示窗口，主要实现窗口样式统一
 *  @file      rmessagebox.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.02
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef RMESSAGEBOX_H
#define RMESSAGEBOX_H

#include <QDialog>
#include <QMessageBox>

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

    void setIcon(QMessageBox::Icon icon);

    static int information(QWidget * parent, const QString &title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    QMessageBox::StandardButton clickedButton()const;

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

    void closeEvent(QCloseEvent * event);

private slots:
    void respButtonClicked();

private:
    RButton * addButton(QMessageBox::StandardButton butt);
    QString standardButtText(QMessageBox::StandardButton butt);

private:
    RMessageBoxPrivate * d_ptr;

};

#endif // RMESSAGEBOX_H
