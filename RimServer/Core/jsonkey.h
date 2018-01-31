/*!
 *  @brief     JSON Key值定义
 *  @details   定义客-服传输结果中，json数据中key字段的内容。
 *             使用了QT5的QMetaEnum对结构体进行解析，获取对应的字符信息。
 *  @file      jsonkey.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.18
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef JSONKEY_H
#define JSONKEY_H

#include <QObject>

class JsonKey : public QObject
{
    Q_OBJECT
public:
    explicit JsonKey(QObject *parent = 0);

    static QString key(int value);

    enum KeyId
    {
        Uuid,
        Type,
        Command,
        Data,
        NickName,
        Pass,
        AccountId,
        SignName,
        Sexual,
        Birth,
        Address,
        Email,
        Phone,
        Remark,
        Face,
        FaceId,
        Status,
        SearchContent,
        SearchType,
        AddType,
        OperateInfo,
        Result,
        OperateId,
        GroupId,
        GroupName,
        IsDefault,
        Users,
        Groups,
        GroupType,
        DestId,
        Time
    };
    Q_FLAG(KeyId)

};

#endif // JSONKEY_H
