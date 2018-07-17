/*!
 *  @brief     自定义文件传送box
 *  @details   文件传送item集合，统一控制内部item以及与其他模块交互
 *  @file      transferfilelistbox.h
 *  @author    LYS
 *  @version   1.0.3
 *  @date      2018.07.12
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *      1.20180713-LYS 统一item于外部交互接口。
 *      2.20180713-LYS 新增item访问接口。
 *      3.20180716-LYS 更新文件传输状态以及多个任务进程。
 *
 */
#ifndef TRANSFERFILELISTBOX_H
#define TRANSFERFILELISTBOX_H

#include <QWidget>
#include "protocol/datastruct.h"
class TransferFileItem;
class TransferFileListBoxPrivate;

class TransferFileListBox : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TransferFileListBox)
public:
    explicit TransferFileListBox(QWidget *parent = 0);

    void addItem(TransferFileItem *);
    QList<TransferFileItem *> items()const;

    void clear();
    bool removeItem(TransferFileItem *);
    bool removeItem(int);

    TransferFileItem *itemAt(int) const;
    int indexOf(TransferFileItem *) const;
    int count() const;

    QString taskListProgress();

signals:
    void itemAdded();
    void itemRemoved();
    void saveAsFile(const QString &);
    void toOffLineSend(const QString &);
    void startRecvFile(const QString &);
    void cancelTransfer(const QString &);
    void transferStatusChanged();

public slots:
    void SetTransStatus(const FileTransProgress &);

private slots:
    void saveAsFile(TransferFileItem *);
    void toOffLineSend(TransferFileItem *);
    void startRecvFile(TransferFileItem *);
    void cancelTransfer(TransferFileItem *);

private:
    TransferFileListBoxPrivate * d_ptr;
};

#endif // TRANSFERFILELISTBOX_H
