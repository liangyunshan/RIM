#include "filedesc.h"

#include <QFile>
#include <QDir>
#include <QDebug>

FileDesc::FileDesc():file(NULL),writeLen(0),size(0)
{

}

bool FileDesc::create()
{
    QString fullPath = filePath + QDir::separator() +fileName;
    file.setFileName(fullPath);
    if(file.open(QFile::WriteOnly)){
        if(file.resize(size)){
            return true;
        }
    }
    return false;
}

bool FileDesc::seek(size_t pos)
{
    if(!isOpen())
        return false;

    return file.seek(pos);
}

qint64 FileDesc::write(const QByteArray &data)
{
    if(!isOpen())
        return -1;

    qint64 realWriteLen = file.write(data);
    writeLen += realWriteLen;
    return realWriteLen;
}

bool FileDesc::flush()
{
    if(isOpen())
        return file.flush();
    return false;
}

bool FileDesc::isRecvOver()
{
    if(!isOpen())
        return false;

    return writeLen == size;
}

void FileDesc::close()
{
    if(isOpen())
        file.close();
}

void FileDesc::destory()
{
    if(file.isOpen()){
        file.close();
    }
}

FileDesc::~FileDesc()
{
    destory();
}

void FileDesc::setFilePath(const QString filePath)
{
    this->filePath = filePath;
}

bool FileDesc::isOpen()
{
    return file.isOpen();
}
