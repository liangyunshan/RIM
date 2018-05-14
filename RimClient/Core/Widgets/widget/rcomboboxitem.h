/*!
 *  @brief     自定义ComboxItem
 *  @details   登陆页面，记录历史登陆记录
 *  @author    wey
 *  @version   1.0
 *  @date      2018.05.05
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef RCOMBOBOXITEM_H
#define RCOMBOBOXITEM_H

#include <QWidget>

class RIconLabel;
class QLabel;
class QPushButton;

class RComboBoxItem : public QWidget
{
    Q_OBJECT
public:
    explicit RComboBoxItem(QWidget * parent = 0);
    ~RComboBoxItem();

    void setNickName(QString name);

    void setPixmap(QString pixmap);

    void setAccountId(QString id);
    QString getAccountId();

protected:
    void mouseReleaseEvent(QMouseEvent *);

signals:
    void deleteItem(QString id);
    void itemClicked(QString id);

private slots:
    void prepareDelete();

private:
    RIconLabel * iconLabel;
    QLabel * nickNameLabel;
    QLabel * accountLabel;
    QPushButton * closeButt;
};

#endif // RCOMBOBOXITEM_H
