#include "widget.h"
#include <QApplication>

#include <iostream>

#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QDateTime>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QSqlDriver>
#include <QMessageBox>

#include "Util/rutil.h"
#include "Util/rlog.h"
#include "rsingleton.h"
#include "constants.h"
#include "thread/recvtextprocessthread.h"
#include "thread/sendtextprocessthread.h"
#include "constants.h"
#include "global.h"
#include "file/xmlparse.h"

#ifdef Q_OS_WIN
#pragma  comment(lib,"ws2_32.lib")
#endif

#include "Network/abstractserver.h"
#include "Network/win32net/tcpserver.h"
using namespace ServerNetwork;

#include "sql/databasemanager.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp")

/*!
 *  @brief 配置文件参数
 */
struct SettingConfig
{
    SettingConfig()
    {
        textRecvProcCount = 5;
        textSendProcCount = 5;
        textListenPort = 8023;
        fileListenPort = 8024;
        textIp = "127.0.0.1";
        fileIp = "127.0.0.1";

        uploadFilePath = qApp->applicationDirPath() + Constant::PATH_File;
    }
    int textRecvProcCount;
    int textSendProcCount;
    unsigned short textListenPort;
    unsigned short fileListenPort;

    QString textIp;
    QString fileIp;

    QString uploadFilePath;
};

inline void CreateMiniDump(PEXCEPTION_POINTERS pep, LPCTSTR strFileName)
{
    HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE))
    {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pep;
        mdei.ClientPointers = NULL;

        ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, (pep != 0) ? &mdei : 0, NULL, 0);

        CloseHandle(hFile);
    }
}

LONG __stdcall MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    CreateMiniDump(pExceptionInfo, L"core.dmp");
    return EXCEPTION_EXECUTE_HANDLER;
}

void parseCommandLine(QApplication & app,CommandParameter & result)
{
    QCommandLineOption serviceOption("s",QCoreApplication::translate("main","choose start service"),"","text");
    serviceOption.setValueName("service");

    QCommandLineOption transOption("m",QCoreApplication::translate("main","choose transmission mode"),"","net");
    transOption.setValueName("trans");

    QCommandLineOption databaseOption("d",QCoreApplication::translate("main","choose database type"),"","mysql");
    databaseOption.setValueName("database");

    QCommandLineParser commandParser;
    commandParser.addOption(serviceOption);
    commandParser.addOption(transOption);
    commandParser.addOption(databaseOption);
    const QCommandLineOption helpOption = commandParser.addHelpOption();
    const QCommandLineOption versionOption = commandParser.addVersionOption();

    if (!commandParser.parse(app.arguments()))
    {
        std::cerr<<commandParser.errorText().toLocal8Bit().data()<<std::endl;
        return;
    }

    if(commandParser.isSet(versionOption))
    {
        printf("%s \n%s %s\n",qPrintable(app.organizationName()), qPrintable(app.applicationName()),
                         qPrintable(app.applicationVersion()));
        result.parseResult = VIEW_PROGRAM;
        return;
    }

    if(commandParser.isSet(helpOption))
    {
        qDebug("%s \t\n"
               "用法: %s.exe [-s type][-m type][-d type][-v][-h] \n\n"
               "服务器支持两种信息服务，包括文本传输和文件传输，用户可使用-s设置服务模式。\n"
               "服务器支持两种通信传输信道，包括网络传输和电话线传输，用户可使用-m设置传输信道。\n"
               "服务器支持两种数据存储，包括MySql和Oracle，用户可使用-d设置数据库类型。\n"
               "\n"
               "选项:\n"
               " -s type \t 设置信息服务的类别,可选类型：\n"
               "         \t [1] text 文本传输服务(默认类型)\n"
               "         \t [2] file 文件传输服务\n"
               " -m type \t 设置传输信道,可选类型：\n"
               "         \t [1] net 以太网传输(默认类型)\n"
               "         \t [2] tel 电话信道传输\n"
               " -d type \t 设置数据库类型，可选类型：\n"
               "         \t [1] mysql msyql数据库(默认类型)\n"
               "         \t [2] oracle Oracle数据库\n"
               " -v      \t 显示程序版本信息\n"
               " -h      \t 显示帮助信息\n"
               "示例:\n"
               "1.以太网传输文本信息： %s.exe -s Text -m Net -d mysql"
               "",app.organizationName().toLocal8Bit().data(),app.applicationName().toLocal8Bit().data(),
               app.applicationName().toLocal8Bit().data());
        result.parseResult = VIEW_PROGRAM;
        return;
    }

    commandParser.process(app.arguments());

    //【1】解析服务类型
    if(commandParser.isSet(serviceOption))
    {
        QString stype = commandParser.value(serviceOption);
        if(stype.toLower() == "text")
        {
            result.serviceType = SERVICE_TEXT;
        }
        else if(stype.toLower() == "file")
        {
            result.serviceType = SERVICE_FILE;
        }
        else
        {
            qDebug("服务类型设置错误! ");
            return;
        }
    }

    //【2】解析传输方式
    if(commandParser.isSet(transOption))
    {
        QString stype = commandParser.value(transOption);
        if(stype.toLower() == "net")
        {
            result.transMode = TRANS_NET;
        }
        else if(stype.toLower() == "tel")
        {
            result.transMode = TRANS_TEL;
        }
        else
        {
            qDebug("传输信道设置错误! ");
            return;
        }
    }

    //【3】解析数据库类型
    if(commandParser.isSet(databaseOption))
    {
        QString dtype = commandParser.value(databaseOption);
        if(dtype.toLower() == "mysql")
        {
            result.dbType = DB_MYSQL;
        }
        else if(dtype.toLower() == "tel")
        {
            result.dbType = DB_ORACLE;
        }
        else
        {
            qDebug("数据库设置错误! ");
            return;
        }
    }

    result.parseResult = EXEC_PROGRAM;
}

void readSettings(QSettings * settings,SettingConfig & localConfig)
{
    //[1]网络配置
    settings->beginGroup(Constant::GroupNetwork);

    if(!settings->contains(Constant::DB_THREAD))
    {
        settings->setValue(Constant::DB_THREAD,localConfig.textRecvProcCount);
    }

    localConfig.textRecvProcCount = settings->value(Constant::DB_THREAD,localConfig.textRecvProcCount).toInt();

    if(!settings->contains(Constant::MSG_THREAD))
    {
        settings->setValue(Constant::MSG_THREAD,localConfig.textSendProcCount);
    }

    localConfig.textSendProcCount = settings->value(Constant::MSG_THREAD,localConfig.textSendProcCount).toInt();

    if(!settings->contains(Constant::TEXT_PORT))
    {
        settings->setValue(Constant::TEXT_PORT,localConfig.textListenPort);
    }

    localConfig.textListenPort = settings->value(Constant::TEXT_PORT,localConfig.textListenPort).toInt();

    if(!settings->contains(Constant::FILE_PORT))
    {
        settings->setValue(Constant::FILE_PORT,localConfig.fileListenPort);
    }

    localConfig.fileListenPort = settings->value(Constant::FILE_PORT,localConfig.fileListenPort).toInt();

    if(!settings->contains(Constant::TEXT_IP))
    {
        settings->setValue(Constant::TEXT_IP,localConfig.textIp);
    }

    localConfig.textIp = settings->value(Constant::TEXT_IP,localConfig.textIp).toString();

    if(!settings->contains(Constant::FILE_IP))
    {
        settings->setValue(Constant::FILE_IP,localConfig.fileIp);
    }

    localConfig.fileIp = settings->value(Constant::FILE_IP,localConfig.fileIp).toString();

    settings->endGroup();

    //[2]文件服务器配置
    settings->beginGroup(Constant::FileServerSetting);

    if(!settings->contains(Constant::UPLOAD_FILE_PATH))
    {
        settings->setValue(Constant::UPLOAD_FILE_PATH,localConfig.uploadFilePath);
    }

    localConfig.uploadFilePath = settings->value(Constant::UPLOAD_FILE_PATH,localConfig.uploadFilePath).toString();

    settings->endGroup();

    settings->sync();
}

void printProgramInfo(CommandParameter & result,QString ip,unsigned short port)
{
    QString serviceType;
    switch(result.serviceType)
    {
        case SERVICE_TEXT: serviceType = "Text Service";break;
        case SERVICE_FILE: serviceType = "File Service";break;
        default:break;
    }

    QString transType;
    switch(result.transMode)
    {
        case TRANS_NET: transType = "Net Trans";break;
        case TRANS_TEL: transType = "Tel Trans";break;
        default:break;
    }

    QString dbType;
    switch(result.dbType)
    {
        case DB_MYSQL: dbType = "MySQL";break;
        case DB_ORACLE: dbType = "Oracle";break;
        default:break;
    }

    qDebug(""
            "       ____                            \n"
            "      / __ \___  ____  ____ ___  __    \n"
            "     / /_/ / _ \/ __ \/ __ `/ / / /    \n"
            "    / _, _/  __/ / / / /_/ / /_/ /     \n"
            "   /_/ |_|\___/_/ /_/\__, /\__,_/      \n"
            "                    /____/             \n"
            "                                       \n"
            "[System working type: %s  %s  %s ]     \n"
            "[Listening ip and port: %s %d]         \n"
           ,serviceType.toLocal8Bit().data(),transType.toLocal8Bit().data(),dbType.toLocal8Bit().data(),ip.toLocal8Bit().data(),port);
}

int main(int argc, char *argv[])
{
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    QApplication a(argc, argv);

    QApplication::setApplicationName(Constant::ApplicationName);
    QApplication::setOrganizationDomain(QObject::tr("rengu.com"));
    QApplication::setOrganizationName(QObject::tr("NanJing RenGu"));
    QApplication::setApplicationVersion(Constant::Version);

    CommandParameter commandResult;
    parseCommandLine(a,commandResult);

#if QT_VERSION > 0x050000
    QTextCodec * codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#else

#endif

    if(commandResult.parseResult == PARSE_ERROR)
    {
        return -1;
    }
    else if(commandResult.parseResult == VIEW_PROGRAM)
    {
        return 0;
    }
    else if(commandResult.parseResult == EXEC_PROGRAM)
    {
        qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

        QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);

        QSettings * settings = new QSettings(configFullPath+"/config.ini",QSettings::IniFormat);
        RUtil::setGlobalSettings(settings);

        if(!RSingleton<RLog>::instance()->init())
        {
            QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Log module initialization failure!"),QMessageBox::Yes,QMessageBox::Yes);
        }

        SettingConfig settingConfig;
        readSettings(settings,settingConfig);

#ifdef __LOCAL_CONTACT__
        RGlobal::G_ParaSettings = new ParameterSettings::ParaSettings;
        QString localConfigName = configFullPath + QDir::separator() + QStringLiteral("参数配置.txt");
        if(!RSingleton<XMLParse>::instance()->parseParaSettings(localConfigName,RGlobal::G_ParaSettings)){
            QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Paramter settings read failed,system exit!"),
                                 QMessageBox::Yes,QMessageBox::Yes);
            exit(-1);
        }

        RGlobal::G_RouteSettings = new ParameterSettings::RouteSettings;
        localConfigName = configFullPath + QDir::separator() + QStringLiteral("路由表.txt");
        if(!RSingleton<XMLParse>::instance()->parseRouteSettings(localConfigName,RGlobal::G_RouteSettings)){
            QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Route settings read failed,system exit!"),
                                 QMessageBox::Yes,QMessageBox::Yes);
            exit(-1);
        }
#endif

        RGlobal::G_SERVICE_TYPE = commandResult.serviceType;

        if(RGlobal::G_SERVICE_TYPE == SERVICE_FILE)
        {
            QDir fileDir(settingConfig.uploadFilePath);
            if(!fileDir.mkpath(settingConfig.uploadFilePath))
            {
                RLOG_ERROR("create file path error ! %s",settingConfig.uploadFilePath.toLocal8Bit().data());
                return -1;
            }
        }
        RGlobal::G_FILE_UPLOAD_PATH = settingConfig.uploadFilePath;

        RSingleton<DatabaseManager>::instance()->setConnectInfo("localhost","rimserver","root","rengu123456");
        RSingleton<DatabaseManager>::instance()->setDatabaseType(commandResult.dbType);

        memset((char *)&RGlobal::G_DB_FEATURE,0,sizeof(Datastruct::DBFeature));
        RGlobal::G_DB_FEATURE.lastInsertId = RSingleton<DatabaseManager>::instance()->hasFeature(QSqlDriver::LastInsertId);

#ifdef DB_TRANSACTIONS_CHECK
        RGlobal::G_DB_FEATURE.transactions = RSingleton<DatabaseManager>::instance()->hasFeature(QSqlDriver::Transactions);
        if(!RGlobal::G_DB_FEATURE.transactions){
            QMessageBox::warning(nullptr,QObject::tr("warning"),QObject::tr("Current database don't support transactions"));
        }
#endif

        if(commandResult.serviceType == SERVICE_TEXT)
        {
            printProgramInfo(commandResult,settingConfig.textIp,settingConfig.textListenPort);
            AbstractServer * tcpTextServer = new TcpServer();
            tcpTextServer->startMe(settingConfig.textIp.toLocal8Bit().data(),settingConfig.textListenPort);
        }
        else if(commandResult.serviceType == SERVICE_FILE)
        {
            printProgramInfo(commandResult,settingConfig.fileIp,settingConfig.fileListenPort);
            AbstractServer * fileServer = new TcpServer();
            fileServer->startMe(settingConfig.fileIp.toLocal8Bit().data(),settingConfig.fileListenPort);
        }

        for(int i = 0; i < settingConfig.textRecvProcCount;i++)
        {
            RecvTextProcessThread * thread = new RecvTextProcessThread;
            Database * dbs = RSingleton<DatabaseManager>::instance()->newDatabase();
            if(!dbs->isError())
            {
                thread->setDatabase(dbs);
                thread->start();
            }
            else
            {
                RLOG_ERROR(dbs->errorInfo().toLocal8Bit().data());
                return -1;
            }
        }

        for(int i = 0; i< settingConfig.textSendProcCount;i++)
        {
            SendTextProcessThread * thread = new SendTextProcessThread;
            thread->start();
        }

        return a.exec();
    }
}
