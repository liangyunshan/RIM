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

    static int information(QWidget * parent,const QString &title,const QString& text,int button0, int button1,int button2 = 0);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

    void closeEvent(QCloseEvent * event);

private:
    RMessageBoxPrivate * d_ptr;

};

#endif // RMESSAGEBOX_H
