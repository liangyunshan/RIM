#include "historybridge.h"

#include "historymsgrecord.h"

void HistoryBridge::setUi(HistoryMsgRecord *ui)
{
    history = ui;
}

void HistoryBridge::playVoiceMsg(const QString &voicePath)
{
    Q_UNUSED(voicePath)
}

void HistoryBridge::openTargetFile(const QString &filePath)
{
    Q_UNUSED(filePath)
    history->openTargetFile(filePath);
}

void HistoryBridge::openTargetFolder(const QString &filePath)
{
    Q_UNUSED(filePath)
    history->openTargetFolder(filePath);
}

void HistoryBridge::transpondFile(const QString &filePath)
{
    Q_UNUSED(filePath)
}
