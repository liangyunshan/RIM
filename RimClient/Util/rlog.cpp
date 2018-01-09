#include "rlog.h"

#include <QApplication>
#include <QDir>
#include <QDate>
#include <QMetaEnum>
#include <QThread>
#include <stdarg.h>

#include "rutil.h"

#include "Core/head.h"

const char PATH_LogPath[] = "/../logs";
const char Suffix[] = ".log";

#define MAX_LOG_LENGH 1024

bool RLog::isFileReady = false;
bool RLog::isRecord2File = true;
RLog::RLOG_LEVEL RLog::logLevel = RLog::INFO;           //默认是info级

QFile localFile;

RLog::RLog()
{
}

void RLog::setLogLevel(const RLog::RLOG_LEVEL &level)
{
    logLevel = level;
}

bool RLog::init()
{
    if(!RUtil::globalSettings()->value("log/log2File").isValid())
    {
        RUtil::globalSettings()->setValue("log/log2File",isRecord2File);
    }
    isRecord2File = RUtil::globalSettings()->value("log/log2File").toBool();

    if(!RUtil::globalSettings()->value("log/logLevel").isValid())
    {
        RUtil::globalSettings()->setValue("log/logLevel",(int)logLevel);
    }

    logLevel = (RLog::RLOG_LEVEL)RUtil::globalSettings()->value("log/logLevel").toInt();

    QString logDir = qApp->applicationDirPath() +  QString(PATH_LogPath);
    if(RUtil::createDir(logDir))
    {
        QString fileName = logDir + QDir::separator() + RUtil::getDate() + QString(Suffix);
        localFile.setFileName(fileName);
        if(localFile.open(QFile::WriteOnly|QFile::Append|QFile::Text))
        {
            isFileReady = true;
        }
    }
    return isFileReady;
}

void RLog::log(RLOG_LEVEL nLevel, const char *fileDesc, const char *functionDesc, int lineNum, const char* data, ...)
{
    if(isFileReady && nLevel >= logLevel)
    {
        QString recordInfo = QString("[%1]").arg(RUtil::getTimeStamp());
        recordInfo.append(getLeveDesc(nLevel));

        recordInfo.append(QString("[%1:%2:%3]").arg(fileDesc).arg(functionDesc).arg(lineNum));

        va_list vlist;
        va_start(vlist,data);

#if defined(Q_OS_WIN)
        int recordLen = _vscprintf(data,vlist);

        QByteArray byteArray;
        byteArray.resize(recordLen);

        vsprintf(byteArray.data(),data,vlist);

        recordInfo.append(byteArray);
#endif
        va_end(vlist);

        recordInfo.append("\n");

        if(isRecord2File)
        {
            localFile.write(recordInfo.toLocal8Bit().data(),recordInfo.toLocal8Bit().length());
            localFile.flush();
        }
        else
        {
//            qDebug()<<recordInfo;
        }
    }
}

QString RLog::getLeveDesc(RLog::RLOG_LEVEL level)
{
#if (QT_VERSION > 0x050500)
    static QMetaEnum metaEnum = QMetaEnum::fromType<RLog::RLOG_LEVEL>();
    return QString("[%1]").arg(metaEnum.key(level));
#else
    switch(level)
    {
        case RLOG_LEVEL::INFO:
                                return "[INFO]";
        case RLOG_LEVEL::INFO:
                                return "[WARNING]";
        case RLOG_LEVEL::INFO:
                                return "[ERROR]";
    }
#endif
}
