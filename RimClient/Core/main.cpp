#include <QApplication>

#include <QTextCodec>
#include <QTranslator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QSettings>

#include "Util/rutil.h"
#include "Widgets/logindialog.h"
#include "constants.h"
#include "Util/rlog.h"
#include "rsingleton.h"
#include "Util/rlog.h"
#include "Util/imagemanager.h"
#include "protocoldata.h"
#include "global.h"
using namespace ProtocolType;

#include "Widgets/nativewindow/MainWindow.h"
#include "Widgets/widget/rmessagebox.h"

#include "thread/msgreceiveproctask.h"
#include "thread/taskmanager.h"

#include "Network/rsocket.h"
#include "Network/msgsender.h"
#include "Network/msgreceive.h"
#include "Network/netconnector.h"

/*!
  运行目录结构
  [+]RIM
      [+]bin
           *.exe
           *.dll
      [+]config
           [+]translations
           [+]skin
           config.xml
      [+]lib
           *.lib
      [+]users
*/

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Background color
//    HBRUSH windowBackground = CreateSolidBrush( RGB( 255, 0, 255 ) );

//    BorderlessWindow window(windowBackground, 100, 100, 1024, 768 );
//    window.setMinimumSize( 800, 600 );

#if QT_VERSION_MAJOR > 4
    QTextCodec * codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
#else
    QTextCodec * codec = QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForLocale(codec);
#endif

    QApplication::setApplicationName(Constant::ApplicationName);
    QApplication::setOrganizationDomain(QObject::tr("rengu.com"));
    QApplication::setOrganizationName(QObject::tr("NanJing RenGu"));
    QApplication::setWindowIcon(QIcon(RSingleton<ImageManager>::instance()->getWindowIcon()));
    QApplication::setFont(QFont("微软雅黑",9));
    QApplication::setQuitOnLastWindowClosed(false);

    QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);

    QSettings * settings = new QSettings(configFullPath+"/config.ini",QSettings::IniFormat);
    RUtil::setGlobalSettings(settings);

    RUtil::createDir(configFullPath);
    RUtil::createDir(qApp->applicationDirPath() + QString(Constant::PATH_UserPath));
    RUtil::createDir(configFullPath + QString(Constant::PATH_StylePath));

    if(!RSingleton<RLog>::instance()->init())
    {
        RMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Log module initialization failure!"),RMessageBox::Yes,RMessageBox::Yes);
    }

    QTranslator translator;

    QString translationPath = configFullPath + QString(Constant::PATH_LocalePath);
    if(RUtil::createDir(translationPath))
    {
        QStringList uiLanguages;

#if (QT_VERSION >= 0x040801) || (QT_VERSION >= 0x040800 && !defined(Q_OS_WIN))
        uiLanguages = QLocale::system().uiLanguages();
#endif

        foreach(QString locale, uiLanguages)
        {
#if (QT_VERSION >= 0x050000)
            locale = QLocale(locale).name();
#else
            locale.replace(QLatin1Char('-'), QLatin1Char('_'));
#endif
            if(translator.load(QString(Constant::ApplicationName)+"_"+ locale,translationPath))
            {
                app.installTranslator(&translator);
                app.setProperty("rimLocale", locale);
                break;
            }
        }
    }

    QFile styleFile(configFullPath + QString(Constant::PATH_StylePath)+"/RimClient.qss");
    if(styleFile.open(QFile::ReadOnly))
    {
        app.setStyleSheet(styleFile.readAll());
    }

    qRegisterMetaType<RegistResponse>("RegistResponse");
    qRegisterMetaType<ResponseRegister>("ResponseRegister");
    qRegisterMetaType<TextUnit::ChatInfoUnitList>("TextUnit::ChatInfoUnitList");

    RSingleton<TaskManager>::instance()->addTask(new NetConnector());
    RSingleton<TaskManager>::instance()->addTask(new MsgReceiveProcTask());

//    QDateTime dt = QDateTime::currentDateTime();
//    qsrand(dt.time().msec() + dt.time().second()*1000);
//    TcpSocket socket;
//    if(socket.createSocket())
//    {
//        if(socket.connect("127.0.0.1",8023))
//        {
//           MsgSender * sender = new MsgSender(socket);
//           sender->startMe();

//           MsgReceive * receive = new MsgReceive(socket);
//           receive->startMe();
//        }
//    }

    LoginDialog dialog;
    dialog.show();

    return app.exec();
}
