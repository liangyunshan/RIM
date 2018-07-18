#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include <QString>

class AbstractChatMainWidget;

class Document:public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER s_text NOTIFY sendText)

public:
    explicit Document(QObject *parent = nullptr):QObject(parent){}

    void setSendTextText(const QString &text);
    void setUi(AbstractChatMainWidget *ui);

public slots:
    void receiveText(const QString &r_path);
    void getMoreRecord();

signals:
    void sendText(const QString &text);

private:
    void displayMessage(const QString &r_path);
    QString s_text;
    AbstractChatMainWidget *chatWidget;
};

#endif//DOCUMENT_H
