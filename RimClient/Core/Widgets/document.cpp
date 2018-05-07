#include "document.h"

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
void Document::setUi(AbstractChatWidget *ui)
{
    chatWidget = ui;
}
