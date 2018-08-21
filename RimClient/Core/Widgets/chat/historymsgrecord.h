/*!
 *  @brief     历史消息查看显示
 *  @details   显示历史聊天消息
 *  @file      historymsgrecord.h
 *  @author    LYS
 *  @version   1.0.1
 *  @date      2018.06.15
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 *
 */
#ifndef HISTORYMSGRECORD_H
#define HISTORYMSGRECORD_H

#include <QWidget>

#include "../../protocol/datastruct.h"

struct ChatInfoUnit;

class HistoryMsgRecordPrivate;

class HistoryMsgRecord : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(HistoryMsgRecord)
public:

    /*!
     *  @brief 历史消息类型
     */
    enum HisoryType
    {
        ALLHISTORY,         //全部历史消息
        IMAGEHISTORY,       //图片历史消息
        FILEHISTORY         //文件历史消息
    };

    explicit HistoryMsgRecord(QWidget *parent = 0);
    ~HistoryMsgRecord();

    void appendTextMsg(ChatInfoUnit & ,HisoryType pageType=ALLHISTORY);
    void appendImageMsg(ChatInfoUnit & ,HisoryType pageType=ALLHISTORY);
    void appendFileMsg(ChatInfoUnit & ,HisoryType pageType=ALLHISTORY);
    void appendVoiceMsg();
    void appendNoticeMsg(QString ,HisoryType pageType=ALLHISTORY ,NoticeType type=NONOTICE);
    void appendDateMsg(QDate ,HisoryType pageType=ALLHISTORY);
    void clearShow(HisoryType pageType=ALLHISTORY);

    void setUserInfo(const SimpleUserInfo &info);
    void openTargetFile(QString filePath);
    void openTargetFolder(QString filePath);

signals:

public slots:

private slots:
    void finishLoadHTML(bool);

    //响应全部历史记录工具栏按钮
    void respOpenSearch(bool);
    void respChooseDate();
    void respSetDate(const QDate &);
    void respSwitchPage();

private:
    HistoryMsgRecordPrivate * d_ptr;
    void setFontIconFilePath(HisoryType pageType=ALLHISTORY);
    QString spliceAppendTextMsg(const ChatInfoUnit &);
    QString spliceAppendImageMsg(const ChatInfoUnit &);
    QString spliceAppendFileMsg(const ChatInfoUnit &);
    QString spliceAppendVoiceMsg(const QString &);
    QString spliceAppendNoticeMsg(const QString & ,NoticeType type=NONOTICE);
    QString spliceAppendDateLine(const QDate &);
};

#endif // HISTORYMSGRECORD_H
