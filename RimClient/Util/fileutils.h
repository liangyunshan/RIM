/*!
 *  @brief     文本帮助工具类
 *  @details   提供统一的文本创建、保存等操作
 *  @file
 *  @author    wey
 *  @version   1.0
 *  @date      2017.12.XX
 *  @warning
 *  @copyright NanJing RenGu.
 */

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QIODevice>
#include <QCoreApplication>

#include "util_global.h"

class QFile;

namespace FileUtils
{

class UTILSHARED_EXPORT FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(FileSaverBase)
public:
    FileSaverBase();
    ~FileSaverBase();

    QString fileName() const { return m_fileName; }
    bool hasError() const { return m_hasError; }
    QString errorString() const { return m_errorString; }

    virtual bool finalize();

    bool write(const char *data, int len);
    bool write(const QByteArray &bytes);
    bool setResult(bool ok);

protected:
    QFile * m_file;
    QString m_fileName;
    QString m_errorString;
    bool m_hasError;

};

class UTILSHARED_EXPORT FileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(FileSaver)
public:
    explicit FileSaver(const QString &filename, QIODevice::OpenMode mode = QIODevice::NotOpen);
    QFile *file() { return m_file; }

    virtual bool finalize();

private:
};

}

#endif // FILEUTILS_H
