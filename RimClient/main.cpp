#include <QApplication>

#include <QTextCodec>
#include <QTranslator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

#include "Util/rutil.h"
#include "Widgets/logindialog.h"
#include "constants.h"
#include "Util/rlog.h"
#include "Util/rsingleton.h"
#include "Util/rlog.h"
#include "Util/imagemanager.h"

#include "Widgets/nativewindow/MainWindow.h"

/*!
  目录结构
  +RIM
      +bin
      +config
           +translations
           +skin
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

    QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);

    QSettings * settings = new QSettings(configFullPath+"/config.ini",QSettings::IniFormat);
    RUtil::setGlobalSettings(settings);

    RUtil::createDir(configFullPath);
    RUtil::createDir(qApp->applicationDirPath() + QString(Constant::PATH_UserPath));
    RUtil::createDir(configFullPath + QString(Constant::PATH_StylePath));

    if(!RSingleton<RLog>::instance()->init())
    {
        QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Log module initialization failure!"),QMessageBox::Yes,QMessageBox::Yes);
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

    LoginDialog dialog;
    dialog.show();

    return app.exec();
}
