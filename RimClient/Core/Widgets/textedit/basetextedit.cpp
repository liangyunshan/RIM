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
#include <QMetaClassInfo>

#include "Util/rutil.h"
#include "util/rsingleton.h"
#include "json/jsonresolver.h"
#include "global.h"
#include "user/user.h"
#include "constants.h"
#include <QAction>
#include <QMenu>

#include <QDebug>

#define SHOTIMAGE_WIDTH 80                  //截图显示宽度
#define SHOTIMAGE_HEIGHT 80                 //截图显示高度

int count = 0;

BaseTextEdit::BaseTextEdit(QWidget *parent):
    QTextEdit(parent)
{
    this->setStyleSheet("border:0px;border-radius:3px;");
    this->installEventFilter(this);

    m_pCopyAction = new QAction(this);
    m_pCopyAction->setText(tr("paste"));
    connect(m_pCopyAction,SIGNAL(triggered(bool)),this,SLOT(pasteData(bool)));
}

bool BaseTextEdit::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->matches(QKeySequence::Paste))
        {
            if(!this->isReadOnly())
            {
                pasteData(true);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

/*!
 * @brief BaseTextEdit::insertCopyImage 将图片插入到文本框当前位置
 * @param image 待插入的图片
 * @return 图片存放路径
 */
QString BaseTextEdit::insertCopyImage(QImage &image)
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
    cursor.insertImage(image,t_imgName);

    return imgAbsoultPath;
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

/*!
 * @brief 是否有可显示的字符
 * @return 有显示字符则返回true，否则为false
 */
bool BaseTextEdit::isVaiablePlaintext()
{
    QString text = this->toPlainText();
    text = text.trimmed();
    if(text.isEmpty())
    {
        return false;
    }
    if(text.size()<=imageCount())
    {
        return false;
    }
    return true;
}

/*!
 * @brief 获取显示的图形个数
 * @return 图形个数
 */
int BaseTextEdit::imageCount()
{
    return m_inputImgs.count();
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
                emit sigDropFile(fileName);
            }
        }
    }
}

void BaseTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    menu.addAction(m_pCopyAction);
    menu.exec(e->globalPos());
}

void BaseTextEdit::pasteData(bool)
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if(mimeData->hasHtml())
    {
        qDebug() << "Ctrl + V mimeData->hasHtml()";
        m_tempEdit.clear();
        m_tempEdit.insertHtml(mimeData->html());
        this->textCursor().insertText(m_tempEdit.toPlainText());
        m_tempEdit.clear();
    }
    else if(mimeData->hasText())
    {
        qDebug() << "Ctrl + V mimeData->hasText()";
        if(!mimeData->hasUrls())
        {
            this->textCursor().insertText(mimeData->text());
        }
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

    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        for(int index=0;index<urlList.count();index++)
        {
            QString fileName = urlList.at(index).toLocalFile();
            if(!fileName.isEmpty())
            {
                emit sigDropFile(fileName);
            }
        }
    }
}
