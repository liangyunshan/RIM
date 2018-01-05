/*!
 *  @brief     基础文本编辑
 *  @details
 *  @file      basetextedit.h
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.22
 *  @warning
 *  @copyright GNU Public License.
 */
#ifndef BASETEXTEDIT_H
#define BASETEXTEDIT_H

#include <QTextEdit>

namespace TextUnit {
    enum ShowType{
        Type_OnlyHead = 0,
        Type_NotOnlyHead ,
    };
    enum BaseTextEditType{
        Type_Default = 0,           //编辑器类型:默认
        Type_UserHead_Friend,       //编辑器类型:用户头
        Type_UserHead_Me ,          //编辑器类型:用户头
        Type_ChatDetail ,           //编辑器类型:聊天内容
        Type_RecordTime ,           //编辑器类型:记录时间
        Type_Tip                    //编辑器类型:提示信息
    };

    struct UserInfo{
        QString name;
        QString head;
    };//用户描述

    struct InfoUnit{
        UserInfo user;
        QString time;
        QString contents;

        InfoUnit (){
            user.name = "Unknown";
            user.head = "Unknown";
            time = "";
            contents = "";
        }

    };//一条聊天记录描述

}


class BaseTextEdit : public QTextEdit
{
public:
    BaseTextEdit(QWidget * parent = 0 );

    TextUnit::InfoUnit ReadJSONFile(QByteArray byteArray);
    QByteArray WriteJSONFile(TextUnit::InfoUnit unit);

    void imgPathToHtml(QString &path);

protected:
    bool eventFilter(QObject *, QEvent *);

};

#endif // BASETEXTEDIT_H
