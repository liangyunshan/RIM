#include "complextextedit.h"
#include <QDateTime>
#include <QTextFrame>
#include <QMovie>
#include <QUrl>

#include <QDebug>

QHash<QMovie*,QUrl> urls;

ComplexTextEdit::ComplexTextEdit(QWidget *parent):
    BaseTextEdit(parent)
{
    m_ShowType = TextUnit::Type_NotOnlyHead;
    startRecordRowid = 0;
    endRecordRowid = 0;

    QFont frindFont(tr("wryh"), 10, QFont::Bold);
    QFont meFont(tr("wryh"), 10, QFont::Bold);

    m_Type_Default_Format.setForeground(Qt::black);
    m_Type_UserHead_Friend_Format.setForeground(Qt::blue);
    m_Type_UserHead_Friend_Format.setFont(frindFont);
    m_Type_UserHead_Me_Format.setForeground(QBrush(QColor(0,128,64)));
    m_Type_UserHead_Me_Format.setFont(meFont);
    m_Type_UserHead_Me_Format.setVerticalAlignment(QTextCharFormat::AlignMiddle);
    m_Type_ChatDetail_Format.setForeground(Qt::black);
    m_Type_RecordTime_Format.setForeground(Qt::lightGray);
    m_Type_Tip_Format.setForeground(Qt::darkBlue);
}

ComplexTextEdit::~ComplexTextEdit()
{

}

//设置界面需要显示的聊天记录的字体样式
void ComplexTextEdit::setChatFormat(const QTextCharFormat &format, TextUnit::BaseTextEditType type)
{
    switch(type)
    {
        case TextUnit::Type_Default:
        {
            m_Type_Default_Format = format;
        }
            break;
        case TextUnit::Type_UserHead_Friend:
        {
            m_Type_UserHead_Friend_Format = format;
        }
            break;
        case TextUnit::Type_UserHead_Me:
        {
            m_Type_UserHead_Me_Format = format;
        }
            break;
        case TextUnit::Type_ChatDetail:
        {
            m_Type_ChatDetail_Format = format;
        }
            break;
        case TextUnit::Type_RecordTime:
        {
            m_Type_RecordTime_Format = format;
        }
            break;
        case TextUnit::Type_Tip:
        {
            m_Type_Tip_Format = format;
        }
            break;
    }
    updateChatShow();
}

void ComplexTextEdit::setChatFont(const QFont &font, TextUnit::BaseTextEditType type)
{
    switch(type)
    {
        case TextUnit::Type_Default:
        {
            m_Type_Default_Format.setFont(font);
        }
            break;
        case TextUnit::Type_UserHead_Friend:
        {
            m_Type_UserHead_Friend_Format.setFont(font);
        }
            break;
        case TextUnit::Type_UserHead_Me:
        {
            m_Type_UserHead_Me_Format.setFont(font);
        }
            break;
        case TextUnit::Type_ChatDetail:
        {
            m_Type_ChatDetail_Format.setFont(font);
        }
            break;
        case TextUnit::Type_RecordTime:
        {
            m_Type_RecordTime_Format.setFont(font);
        }
            break;
        case TextUnit::Type_Tip:
        {
            m_Type_Tip_Format.setFont(font);
        }
            break;
        default:
        {
            return ;
        }
            break;
    }
    updateChatShow();
}

//添加对方的聊天内容
void ComplexTextEdit::insertFriendChatText(const TextUnit::ChatInfoUnit record)
{
    QTextFrameFormat frameFormat;                  //创建框架格式
    frameFormat.setBackground(Qt::white);           //设置背景色
    frameFormat.setMargin(3);
    frameFormat.setBorder(1);

    QTextBlockFormat blockFormat_head = QTextBlockFormat();
    blockFormat_head.setLeftMargin(1);
    blockFormat_head.setRightMargin(1);
    blockFormat_head.setTopMargin(1);
    blockFormat_head.setBottomMargin(1);

    QTextBlockFormat blockFormat_contents = QTextBlockFormat();
    blockFormat_contents.setLeftMargin(15);
    blockFormat_contents.setRightMargin(15);
    blockFormat_contents.setTopMargin(3);
    blockFormat_contents.setBottomMargin(10);

    //描述聊天信息的用户标识和时间标识等，目前采用固定的格式
    //用户名+聊天信息时间戳
    QString me = QString("%1 %2").arg(record.user.name).arg(record.time);

    this->moveCursor(QTextCursor::End);
    QTextCursor cursor  = this->textCursor();
    cursor.insertBlock(blockFormat_head);
    cursor.insertText(me ,m_Type_UserHead_Friend_Format);
    cursor.insertBlock(blockFormat_contents);
    this->insertChatFormatText(record.contents);
}

//添加自己发送的聊天信息
void ComplexTextEdit::insertMeChatText(const TextUnit::ChatInfoUnit record)
{
    QTextFrameFormat frameFormat;                  //创建框架格式
    frameFormat.setBackground(Qt::white);           //设置背景色
    frameFormat.setMargin(3);
    frameFormat.setBorder(1);

    QTextBlockFormat blockFormat_head = QTextBlockFormat();
    blockFormat_head.setLeftMargin(1);
    blockFormat_head.setRightMargin(1);
    blockFormat_head.setTopMargin(1);
    blockFormat_head.setBottomMargin(1);

    QTextBlockFormat blockFormat_contents = QTextBlockFormat();
    blockFormat_contents.setLeftMargin(15);
    blockFormat_contents.setRightMargin(15);
    blockFormat_contents.setTopMargin(3);
    blockFormat_contents.setBottomMargin(10);

    //描述聊天信息的用户标识和时间标识等，目前采用固定的格式
    //用户名+聊天信息时间戳
    QString me = QString("%1 %2").arg(record.user.name).arg(record.time);

    this->moveCursor(QTextCursor::End);
    QTextCursor cursor  = this->textCursor();
    cursor.insertBlock(blockFormat_head);
    cursor.insertText(me ,m_Type_UserHead_Me_Format);
    cursor.insertBlock(blockFormat_contents);
    this->insertChatFormatText(record.contents);

    //
//    this->moveCursor(QTextCursor::End);
//    this->textCursor().insertText(me ,m_Type_UserHead_Me_Format);
//    this->textCursor().insertFrame(frameFormat_chatinfo);   //在光标处插入框架
//    this->insertPlainText(record.contents);
    //

    compareRowId(record.rowid);
}

//添加标注提示信息
void ComplexTextEdit::insertTipChatText(const QString tip)
{
    this->moveCursor(QTextCursor::End);

    QTextFrameFormat frameFormat;                   //创建框架格式
    frameFormat.setBackground(Qt::gray);            //设置背景色
    frameFormat.setMargin(2);                       //设置边距
    frameFormat.setTopMargin(2);
    frameFormat.setBottomMargin(2);
    frameFormat.setLeftMargin(22);
    frameFormat.setPadding(2);                      //设置填充
    frameFormat.setBorder(0);                       //设置边界宽度
    this->textCursor().insertFrame(frameFormat);    //在光标处插入框架
    this->insertPlainText(tip);
}

void ComplexTextEdit::updateChatShow()
{

}

void ComplexTextEdit::showTextFrame()
{
    QTextDocument * doc = this->document();
    QTextFrame *frame = doc->rootFrame();
    QTextFrame::iterator it;                            //建立QTextFrame类的迭代器
    for (it = frame->begin(); !(it.atEnd()); ++it) {
        QTextFrame * childFrame = it.currentFrame();    //获取当前框架的指针
        QTextBlock childBlock = it.currentBlock();      //获取当前文本块
        if (childFrame)
        {}
        else if (childBlock.isValid())
        {}
    }
}

void ComplexTextEdit::addAnimation(const QUrl& url, const QString& fileName)
{
    QFile *file =new QFile(fileName);  //读取gif文件
        if(!file->open(QIODevice::ReadOnly)){
        qDebug()<<" open err";
    }

//    if(lstUrl.contains(url)){ //同一个gif 使用同一个movie
//        return;
//    }else{
//        lstUrl.append(url);
//    }

    QMovie* movie = new QMovie(this); //
    movie->setFileName(fileName);
    movie->setCacheMode(QMovie::CacheNone);


//    lstMovie.append(movie);   //获取该movie,以便删除
    urls.insert(movie, url);   //urls 一个hash

    //换帧时刷新
    connect(movie, SIGNAL(frameChanged(int)), this, SLOT(animate(int)));
    movie->start();
    file->close();
    delete file;
}

//鼠标滚动显示历史记录信息
void ComplexTextEdit::wheelEvent(QWheelEvent *event)
{
    if(event->delta()>0)
    {
        qDebug()<<__FILE__<<__LINE__<<"\n"
               <<""<<event
              <<"\n";
        //TODO: 查询是否有历史信息，并显示在聊天框
    }
    BaseTextEdit::wheelEvent(event);
}

void ComplexTextEdit::animate(int anim)
{
       // qDebug()<<"hash count is "<<urls.count();
    if (QMovie* movie = qobject_cast<QMovie*>(sender()))
    {
        document()->addResource(QTextDocument::ImageResource,   //替换图片为当前帧
                                urls.value(movie), movie->currentPixmap());

        setLineWrapColumnOrWidth(lineWrapColumnOrWidth()); // ..刷新显示
    }
}

//记录当前聊天记录页面的数据记录索引
void ComplexTextEdit::compareRowId(int rowid)
{
    if(startRecordRowid == 0)
    {
        startRecordRowid = rowid;
    }
    if(endRecordRowid == 0)
    {
        endRecordRowid = rowid;
    }

    if(rowid<startRecordRowid)
    {
        startRecordRowid = rowid;
    }

    if(rowid>endRecordRowid)
    {
        endRecordRowid = rowid;
    }
}
