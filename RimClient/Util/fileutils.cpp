#include "fileutils.h"

#include <QDir>

namespace FileUtils {


FileSaverBase::FileSaverBase():m_hasError(false)
{

}

FileSaverBase::~FileSaverBase()
{
    delete m_file;
}

bool FileSaverBase::finalize()
{
    m_file->close();
    m_hasError = m_file->error() == QFile::NoError;
    delete m_file;
    m_file = NULL;
    return m_hasError;
}

bool FileSaverBase::write(const char *data, int len)
{
    if (m_hasError)
    {
        return false;
    }
    return setResult(m_file->write(data, len) == len);
}

bool FileSaverBase::write(const QByteArray &bytes)
{
    if (m_hasError)
    {
        return false;
    }
    return setResult(m_file->write(bytes) == bytes.count());
}

bool FileSaverBase::setResult(bool ok)
{
    if (!ok && !m_hasError)
    {
        m_errorString = tr("Cannot write file %1. Disk full?").arg(QDir::toNativeSeparators(m_fileName));
        m_hasError = true;
    }
    return ok;
}

FileSaver::FileSaver(const QString &filename, QIODevice::OpenMode mode)
{
    m_fileName = filename;

    m_file = new QFile(filename);

    if (!m_file->open(QIODevice::WriteOnly | mode))
    {
        QString err = QFile::exists(filename) ? tr("Cannot overwrite file %1: %2") : tr("Cannot create file %1: %2");
        m_errorString = err.arg(QDir::toNativeSeparators(filename), m_file->errorString());
        m_hasError = true;
    }
}

bool FileSaver::finalize()
{
    if(!m_hasError)
    {
        if(!m_file->flush())
        {
            m_file->remove();
            return false;
        }

        m_file->close();
        if (m_file->error() != QFile::NoError) {
            m_file->remove();
            return false;
        }

        return true;
    }
    return false;
}


}
