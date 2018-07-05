/*!
 *  @brief     聊天对话窗口
 *  @details   显示双方的聊天内容，支持显示头像、图片、文字样式、历史记录等
 *  @file      complextextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef COMPLEXTEXTEDIT_H
#define COMPLEXTEXTEDIT_H

#include "basetextedit.h"
#include "../../protocol/protocoldata.h"

class ComplexTextEdit : public BaseTextEdit
{
    Q_OBJECT
public:
    ComplexTextEdit(QWidget * parent = 0);
    ~ComplexTextEdit();

    void setChatFormat(const QTextCharFormat &format, BaseTextEditType type);
    void setChatFont(const QFont &font, BaseTextEditType type);

    void insertTipChatText(const QString tip);
    void setSimpleUserInfo(SimpleUserInfo user);

protected:
    void updateChatShow();
    void showTextFrame();
    void addAnimation(const QUrl& url, const QString& fileName);
    void wheelEvent(QWheelEvent *event);

signals:
    void sig_QueryRecordTask(int user_query_id,int currStartRow);

private slots:
    void animate(int anim);

private:
    void compareRowId(int rowid);

private:
    ShowType m_ShowType;
    QTextCharFormat m_Type_Default_Format;
    QTextCharFormat m_Type_UserHead_Friend_Format;
    QTextCharFormat m_Type_UserHead_Me_Format;
    QTextCharFormat m_Type_ChatDetail_Format;
    QTextCharFormat m_Type_RecordTime_Format;
    QTextCharFormat m_Type_Tip_Format;

    int startRecordRowid;
    int endRecordRowid;

    SimpleUserInfo userInfo;               //用户基本信息
};

#endif // COMPLEXTEXTEDIT_H
