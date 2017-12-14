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

/*!
  目录结构
  +RIM
      +bin
      +config
           +translations
           +skin
*/

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName(Constant::ApplicationName);

    QTextCodec * codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);

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

                LOG_INFO("Load translate file");
                LOG_WARNING("Load translate file");
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
