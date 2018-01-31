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

class ComplexTextEdit : public BaseTextEdit
{
public:
    ComplexTextEdit(QWidget * parent = 0);
    ~ComplexTextEdit();

    void setChatFormat(const QTextCharFormat &format, TextUnit::BaseTextEditType type);
    void setChatFont(const QFont &font, TextUnit::BaseTextEditType type);

    void insertFriendChatText(const TextUnit::ChatInfoUnit record);
    void insertMeChatText(const TextUnit::ChatInfoUnit record);
    void insertTipChatText(const QString tip);

protected:
    void updateChatShow();
    void showTextFrame();
    void addAnimation(const QUrl& url, const QString& fileName);
    void wheelEvent(QWheelEvent *event);

private slots:
    void animate(int anim);

private:
    void compareRowId(int rowid);

private:
    TextUnit::ShowType m_ShowType;
    QTextCharFormat m_Type_Default_Format;
    QTextCharFormat m_Type_UserHead_Friend_Format;
    QTextCharFormat m_Type_UserHead_Me_Format;
    QTextCharFormat m_Type_ChatDetail_Format;
    QTextCharFormat m_Type_RecordTime_Format;
    QTextCharFormat m_Type_Tip_Format;

    int startRecordRowid;
    int endRecordRowid;
};

#endif // COMPLEXTEXTEDIT_H
