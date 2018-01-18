#ifndef RIMSQLBASE_H
#define RIMSQLBASE_H

#include <QObject>

class RimSqlBase : public QObject
{
    Q_OBJECT
public:
    explicit RimSqlBase(QObject *parent = nullptr);

signals:

public slots:
};

#endif // RIMSQLBASE_H