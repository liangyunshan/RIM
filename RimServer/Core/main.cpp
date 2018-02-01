#include "widget.h"
#include <QApplication>

#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QDateTime>

#include "Util/rutil.h"
#include "Util/rlog.h"
#include "rsingleton.h"
#include "constants.h"
#include "thread/recvtextprocessthread.h"
#include "thread/sendtextprocessthread.h"

#pragma  comment(lib,"ws2_32.lib")

#include "Network/tcpserver.h"
using namespace ServerNetwork;

#include "sql/databasemanager.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if QT_VERSION > 0x050000
    QTextCodec * codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#else

#endif

    qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());

    QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);

    QSettings * settings = new QSettings(configFullPath+"/config.ini",QSettings::IniFormat);
    RUtil::setGlobalSettings(settings);

    if(!RSingleton<RLog>::instance()->init())
    {
        QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Log module initialization failure!"),QMessageBox::Yes,QMessageBox::Yes);
    }

    DatabaseManager dbManager;
    dbManager.setConnectInfo("localhost","rimserver","root","rengu123456");
    dbManager.setDatabaseType("QMYSQL");

    for(int i = 0; i< 5;i++)
    {
        RecvTextProcessThread * thread = new RecvTextProcessThread;
        thread->setDatabase(dbManager.newDatabase());
        thread->start();
    }

    for(int i = 0; i< 5;i++)
    {
        SendTextProcessThread * thread = new SendTextProcessThread;
        thread->start();
    }

    TcpServer server;
    server.startMe("192.168.0.125",8023);
    return a.exec();
}
