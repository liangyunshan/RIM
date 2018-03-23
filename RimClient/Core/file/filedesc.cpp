#include "filedesc.h"

#include <QFile>
#include <QDir>
#include <QDebug>

FileDesc::FileDesc():file(NULL)
{

}

bool FileDesc::create()
{
    QString fullPath = filePath + QDir::separator() +fileName;
    file = new QFile(fullPath);
    if(file->open(QFile::WriteOnly) )
    {
        if(file->resize(size))
        {
            return true;
        }
    }
    return false;
}

bool FileDesc::seek(size_t pos)
{
    if(!file)
        return false;

    return file->seek(pos);
}

qint64 FileDesc::write(const QByteArray &data)
{
    if(!file)
        return -1;
    qint64 realWriteLen = file->write(data);
    writeLen += realWriteLen;
    return realWriteLen;
}

bool FileDesc::flush()
{
    if(file)
        return file->flush();
    return false;
}

bool FileDesc::isRecvOver()
{
    if(!file)
        return false;
//    qDebug()<<writeLen<<"__File::"<<file->size();
    return writeLen == file->size();
}

void FileDesc::close()
{
    if(file)
        file->close();
}

void FileDesc::destory()
{
    if(file)
    {
        if(file->isOpen())
        {
            file->close();
        }
        delete file;
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
