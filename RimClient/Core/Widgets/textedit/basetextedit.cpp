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
#include <QDir>
#include <QTime>
#include <QTextCursor>
#include <QCryptographicHash>

#include "Util/rutil.h"
#include "rsingleton.h"
#include "json/jsonresolver.h"
#include "global.h"
#include "user/user.h"
#include "constants.h"

#include <QDebug>

#define SHOTIMAGE_WIDTH 80                  //截图显示宽度
#define SHOTIMAGE_HEIGHT 80                 //截图显示高度

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
    unit.user.id = G_User->BaseInfo().accountId.toInt();
    unit.user.name = G_User->BaseInfo().nickName;
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

/*!
 * @brief BaseTextEdit::insertCopyImage 将图片插入到文本框当前位置
 * @param image 待插入的图片
 * @return 无
 */
void BaseTextEdit::insertCopyImage(QImage &image)
{
    QString t_msec = QString::number(QTime::currentTime().msec());
    QByteArray data = QByteArray((char*)image.bits()).append(t_msec.toLocal8Bit());
    QByteArray imgId = QCryptographicHash::hash (data , QCryptographicHash::Md5 );
    QString t_imgName ;
    t_imgName.append(imgId.toHex());
    t_imgName += QString(".png");

    //保存图片到本地
    QString imgAbsoultPath = G_User->getC2CImagePath() + QDir::separator() + t_imgName;

    QFileInfo fileinfo(imgAbsoultPath);
    if(!fileinfo.exists())
    {
        image.save(imgAbsoultPath);
    }
    m_inputImgs.append(imgAbsoultPath);

    //图片比例缩放显示
    int width = SHOTIMAGE_WIDTH;
    if (image.width() > width || image.height() > width)
    {
        image = image.scaled(SHOTIMAGE_WIDTH,SHOTIMAGE_HEIGHT,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    }
    this->document()->addResource(QTextDocument::ImageResource,QUrl("image"),image);
    QTextCursor cursor = this->textCursor();
    //获取登录账号id用于拼接HTML中的img标签内容
    QString imgFlagContent = QString("");
    QString userID = G_User->BaseInfo().accountId;
    imgFlagContent = imgFlagContent + Constant::PATH_UserPath + QDir::separator() + userID + QDir::separator() + Constant::USER_RecvFileDirName;
    imgFlagContent = imgFlagContent + QDir::separator() + Constant::USER_ChatImageDirName + QDir::separator() + Constant::USER_C2CDirName;
    imgFlagContent = imgFlagContent + QDir::separator() + t_imgName;
//    cursor.insertImage(image,imgFlagContent);
    cursor.insertImage(image,t_imgName);    //使用纯文件名作为img标签内容
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

/*!
 * @brief BaseTextEdit::getInputedImgs 获取当前文本输入框中全部图片保存路径
 * @param imgDirs 保存所有图片路径的列表
 */
void BaseTextEdit::getInputedImgs(QStringList &imgDirs)
{
    imgDirs = m_inputImgs;
}

void BaseTextEdit::clearInputImg()
{
    m_inputImgs.clear();
}

void BaseTextEdit::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
    }
    else
    {
        e->ignore();
    }
}

//处理拖拽进来的文件
void BaseTextEdit::dropEvent(QDropEvent *e)
{
    const QMimeData *mimeData = e->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        for(int index=0;index<urlList.count();index++)
        {
            QString fileName = urlList.at(index).toLocalFile();
            if(!fileName.isEmpty())
            {
                QFileInfo filInfo(fileName);
                Q_UNUSED(filInfo);
                //TODO 发送拖拽进来的文件
            }
        }
    }
}
