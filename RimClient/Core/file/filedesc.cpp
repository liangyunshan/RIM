#include "filedesc.h"

#include <QFile>
#include <QDir>
#include <QDebug>

FileRecvDesc::FileRecvDesc():file(NULL)
{

}

bool FileRecvDesc::create()
{
    QString fullPath = saveFilePath + QDir::separator() +fileName;
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

bool FileRecvDesc::seek(size_t pos)
{
    if(!file)
        return false;

    return file->seek(pos);
}

qint64 FileRecvDesc::write(const QByteArray &data)
{
    if(!file)
        return -1;
    qint64 realWriteLen = file->write(data);
    writeLen += realWriteLen;
    return realWriteLen;
}

bool FileRecvDesc::flush()
{
    if(file)
        return file->flush();
    return false;
}

bool FileRecvDesc::isRecvOver()
{
    if(!file)
        return false;
//    qDebug()<<writeLen<<"__File::"<<file->size();
    return writeLen == file->size();
}

void FileRecvDesc::close()
{
    if(file)
        file->close();
}

void FileRecvDesc::destory()
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
FileRecvDesc::~FileRecvDesc()
{
    destory();
}

void FileRecvDesc::setFilePath(const QString filePath)
{
    saveFilePath = filePath;
}
