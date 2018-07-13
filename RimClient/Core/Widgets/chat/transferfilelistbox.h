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

public slots:
    void SetTransStatus(FileTransProgress);

private slots:
    void saveAsFile(TransferFileItem *);
    void toOffLineSend(TransferFileItem *);
    void startRecvFile(TransferFileItem *);
    void cancelTransfer(TransferFileItem *);

private:
    TransferFileListBoxPrivate * d_ptr;
};

#endif // TRANSFERFILELISTBOX_H
