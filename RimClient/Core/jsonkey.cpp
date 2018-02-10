#include "jsonkey.h"

#include <QMetaEnum>

JsonKey::JsonKey(QObject *parent) : QObject(parent)
{

}

/*!
 * @brief 获取对应类型字符串
 * @details 利用Qt5中的元对象系统，将枚举值的key直接转换成字符串。
 * @param[in] value 枚举值
 * @return 对应枚举值的字符串
 */
QString JsonKey::key(int value)
{
    static QMetaEnum metaEnum = QMetaEnum::fromType<KeyId>();

    return metaEnum.key(value)?QString(metaEnum.key(value)):QString();
}
