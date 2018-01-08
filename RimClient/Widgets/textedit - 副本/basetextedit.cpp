#include "basetextedit.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QEvent>
#include <QKeyEvent>

#include <QDebug>

BaseTextEdit::BaseTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    this->installEventFilter(this);
}

//解析接收的json数据
TextUnit::InfoUnit BaseTextEdit::ReadJSONFile(QByteArray byteArray)
{
    TextUnit::InfoUnit unit;

    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);
    if((!doucment.isNull())
            && (jsonError.error == QJsonParseError::NoError))
    {
        if(doucment.isObject())
        {
            QJsonObject obj = doucment.object();
            if(obj.contains("UserName"))
            {
                unit.user.name = obj.value("UserName").toString();
            }
            if(obj.contains("UserHead"))
            {
                unit.user.head = obj.value("UserHead").toString();
            }
            if(obj.contains("RecordTime"))
            {
                unit.time = obj.value("RecordTime").toString();
            }
            if(obj.contains("RecordContents"))
            {
                unit.contents = obj.value("RecordContents").toString();
            }
        }
    }

    return unit;
}

//编辑json数据发送
QByteArray BaseTextEdit::WriteJSONFile(TextUnit::InfoUnit unit)
{
    // 构建 Json 对象
    QJsonObject Unit;
    Unit.insert("UserName", unit.user.name);
    Unit.insert("UserHead", unit.user.head);
    Unit.insert("RecordTime", unit.time);
    Unit.insert("RecordContents", unit.contents);

    // 构建 JSON 文档
    QJsonDocument document;
    document.setObject(Unit);
    QByteArray byteArray = document.toJson(QJsonDocument::Indented);
    return byteArray;
}

//将图片的路径转化为HTML格式路径
void BaseTextEdit::imgPathToHtml(QString &path)
{
    path = QString("<img src=\"%1\"/>").arg(path);
}

bool BaseTextEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::Paste))
        {
            qDebug() << "Ctrl + V 2";
        }
    }
    return QWidget::eventFilter(obj, event);
}
