#ifndef HISTORYBRIDGE_H
#define HISTORYBRIDGE_H

#include <QObject>
#include <QString>

class HistoryMsgRecord;

class HistoryBridge :public QObject
{
    Q_OBJECT
public:
    explicit HistoryBridge(QObject *parent = nullptr):QObject(parent){}

    void setUi(HistoryMsgRecord *ui);

public slots:
    void playVoiceMsg(const QString &voicePath);
    void openTargetFile(const QString &filePath);
    void openTargetFolder(const QString &filePath);
    void transpondFile(const QString &filePath);

private:
    HistoryMsgRecord *history;
};

#endif // HISTORYBRIDGE_H
