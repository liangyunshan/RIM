#include "jsonkey.h"

#include <QMetaEnum>

JsonKey::JsonKey(QObject *parent) : QObject(parent)
{

}

QString JsonKey::key(int value)
{
    static QMetaEnum metaEnum = QMetaEnum::fromType<KeyId>();

    return metaEnum.key(value)?QString(metaEnum.key(value)):QString();
}
