#include "document.h"

#include "abstractchatmainwidget.h"

#include <QDir>
#include <QFileInfo>

#include <QDebug>

void Document::setSendTextText(const QString &text) //向html页面发送数据
{
    s_text = text;
    emit sendText(s_text);
}
void Document::displayMessage(const QString &r_path)//接收来自html页面的数据
{
    chatWidget->playVoiceMessage(r_path);
}
/*!
    This slot is invoked from the HTML client side and the text displayed on the server side.
*/
void Document::receiveText(const QString &r_path)
{
    displayMessage(r_path);
}

void Document::getMoreRecord()
{
    chatWidget->updateMsgRecord();
}

bool Document::targetIsDir(const QString &path)
{
    QFileInfo t_fileInfo(path);
    return t_fileInfo.isDir();
}

QString Document::getTargetName(const QString &path)
{
    QFileInfo t_fileInfo(path);
    if(t_fileInfo.isDir())
    {
        return t_fileInfo.dir().dirName();
    }
    else
    {
        return t_fileInfo.fileName();
    }
}

QString Document::getTargetDir(const QString &path)
{
    QFileInfo t_fileInfo(path);
    return t_fileInfo.path();
}

void Document::openFile(const QString &filePath)
{
    chatWidget->openTargetFile(filePath);
}

void Document::openFolder(const QString &filePath)
{
    chatWidget->openTargetFolder(filePath);
}

void Document::setUi(AbstractChatMainWidget *ui)
{
    chatWidget = ui;
}
