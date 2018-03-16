#include "basetextedit.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QEvent>
#include <QKeyEvent>
#include <QClipboard>
#include <QMimeData>
#include <QCursor>
#include <QApplication>
#include <QDateTime>
#include <QXmlStreamReader>
#include <QFile>
#include <QBuffer>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QDir>
#include <QTextCursor>

#include "Util/rutil.h"
#include "rsingleton.h"
#include "json/jsonresolver.h"
#include "global.h"

#include <QDebug>

BaseTextEdit::BaseTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    this->setStyleSheet("border:0px;border-radius:3px;");
    this->installEventFilter(this);
}

bool BaseTextEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::Paste))
        {
            if(!this->isReadOnly())
            {
                QClipboard *clipboard = QApplication::clipboard();
                const QMimeData *mimeData = clipboard->mimeData();

                if(mimeData->hasHtml())
                {
                    qDebug() << "Ctrl + V mimeData->hasHtml()";
                    this->textCursor().insertHtml(mimeData->html());
                }
                else if(mimeData->hasText())
                {
                    qDebug() << "Ctrl + V mimeData->hasText()";
                    this->textCursor().insertText(mimeData->text());
                }
                if(mimeData->hasImage())
                {
                    qDebug() << "Ctrl + V mimeData->hasImage()";
                    QImage image = qvariant_cast<QImage>(mimeData->imageData());

                    insertCopyImage(image);
                }

                if(mimeData->hasColor())
                {
                    qDebug() << "Ctrl + V mimeData->hasColor()";
                }
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

//将内容打包成网络包 ChatInfoUnit
int BaseTextEdit::transTextToUnit(TextUnit::ChatInfoUnit &unit)
{
    QFont font = this->font();
    QColor color = this->textColor();
    QString contents = this->toChatFormaText().replace("'","\"");

    unit.rowid = -1;
    unit.user.id = G_UserBaseInfo.accountId.toInt();
    unit.user.name = G_UserBaseInfo.nickName;
    unit.user.head = ":/icon/resource/icon/person_1.png"; //用户头像
    unit.time = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
    unit.font.fontName = font.key();
    unit.font.fontSize = font.pointSize();
    unit.font.fontIsBold = font.bold();
    unit.font.fontIsItalic = font.italic();
    unit.font.fontIsUnderline = font.underline();
    unit.font.fontColorRGBA = color.rgba();
    unit.contents = contents;

    return 0;
}

//[1] 界面显示的数据->封装网络聊天数据 string->format
QString BaseTextEdit::toChatFormaText()
{
    QString ui_html = toHtml();
    QString contents ;
    RSingleton<JsonResolver>::instance()->parseHtml(contents,ui_html,TextUnit::Parase_Send);
    return contents;
}

//[2] 将网络封装的聊天数据->解析为界面可以显示的数据 format->string
void BaseTextEdit::insertChatFormatText(const QString &format)
{
    //解析数据，显示在界面
    QString contents ;
    RSingleton<JsonResolver>::instance()->parseHtml(contents,format,TextUnit::Parase_Recv);
    this->insertHtml(contents);
}

int BaseTextEdit::insertCopyImage(QImage &image)
{
    QByteArray bb = QCryptographicHash::hash ( QByteArray((char*)image.bits()), QCryptographicHash::Md5 );
    QString temp_img_filepath ;
    temp_img_filepath.append(bb.toHex());
    QString imgfilepath = G_Temp_Picture_Path + "/" + temp_img_filepath + ".png";
    QFileInfo fileinfo(imgfilepath);
    if(!fileinfo.exists())
    {
        image.save(imgfilepath);
    }

    int width = this->viewport()->width() - 7;
    if (image.size().width() > width || image.size().height() > width)
    {
        image = image.scaled(width, width, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QTextImageFormat imageFormat;;
    imageFormat.setName(imgfilepath);
    this->textCursor().insertImage(imageFormat);

    return 0;
}

void BaseTextEdit::setInputTextColor(QColor Color)
{
    if(Color.isValid())
    {
        this->selectAll();

        QTextCharFormat tcf = this->textCursor().charFormat();
        tcf.setForeground(Color);
        this->textCursor().mergeCharFormat(tcf);

        this->moveCursor(QTextCursor::End);
    }
}

void BaseTextEdit::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
    {
        e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
    }
}

//处理拖拽进来的文件
void BaseTextEdit::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();

    foreach (QUrl u, urls) {
        this->insertPlainText(QString("%1 - %2").arg(u.toString()).arg("Drag File"));
    }

    if(fileName.isEmpty())
        return;

    //TODO:发送该文件
}
