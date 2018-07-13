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

    void addItem(TransferFileItem * item);
    QList<TransferFileItem *> items()const;

    void clear();
    bool removeItem(TransferFileItem * item);

    int count() const;

signals:

public slots:
    void SetTransStatus(FileTransProgress);

private:
    TransferFileListBoxPrivate * d_ptr;
};

#endif // TRANSFERFILELISTBOX_H
