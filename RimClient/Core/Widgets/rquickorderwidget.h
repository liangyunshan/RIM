/*!
 *  @brief     快捷命令类
 *  @details   支持生成默认快捷命令、添加删除快捷命令、保存为本地快捷命令文件
 *  @file      rquickorderwidget.h
 *  @author    尚超
 *  @version   1.0
 *  @date      2018.08.08
 *  @copyright NanJing RenGu.
 */
#ifndef RQUICKORDERWIDGET_H
#define RQUICKORDERWIDGET_H

#include "widget.h"
#include "observer.h"

class RQuickOrderWidgetPrivate;

class RQuickOrderWidget : public Widget,public Observer
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RQuickOrderWidget)
public:
    explicit RQuickOrderWidget(QWidget *parent = 0);
    ~RQuickOrderWidget();

    static RQuickOrderWidget * instance();

    void addOrder(QString order=QString());
    void clearOrder();
    int orderCount();
    QStringList getOrderList();
    void readLocalOrderFile();
    void writeOrderToLocalFile();
    void initOrderFilePath();
    void setOrderFilePath(QString path);
    void onMessage(MessageType type);

signals:
    void setComplete(QStringList);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void addItem(bool);
    void deleteItem(bool);

private:
    RQuickOrderWidgetPrivate * d_ptr;
    QString m_userHome;
    QString m_filePath;
    void creatDefaultOrderFile();
};

#endif // RQUICKORDERWIDGET_H
