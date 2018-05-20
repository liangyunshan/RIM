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
#include "application.h"
using namespace ProtocolType;

#include "Widgets/nativewindow/MainWindow.h"
#include "Widgets/widget/rmessagebox.h"

#include "thread/taskmanager.h"

#include "Network/rsocket.h"
#include "Network/win32net/msgsender.h"
#include "Network/win32net/msgreceive.h"

#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp")

/*!
  运行目录结构
  [+]RIM
      [+]bin
           *.exe
           *.dll
      [+]config
           [+]translations
           [+]skin
           [+]sound
           config.xml
      [+]lib
           *.lib
      [+]users
*/

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

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


int main(int argc, char *argv[])
{
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    Application app(argc, argv);

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

    //设置系统工作路径
    QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);

    QSettings * settings = new QSettings(configFullPath+"/config.ini",QSettings::IniFormat);
    RUtil::setGlobalSettings(settings);

    RUtil::createDir(configFullPath);
    RUtil::createDir(qApp->applicationDirPath()  + QDir::separator() + QString(Constant::PATH_UserPath));
    RUtil::createDir(configFullPath + QString(Constant::CONFIG_StylePath));

    if(!RSingleton<RLog>::instance()->init())
    {
        RMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Log module initialization failure!"),RMessageBox::Yes,RMessageBox::Yes);
    }

    QTranslator translator;

    QString translationPath = configFullPath + QString(Constant::CONFIG_LocalePath);
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

    QFile styleFile(configFullPath + QString(Constant::CONFIG_StylePath)+"/RimClient.qss");
    if(styleFile.open(QFile::ReadOnly))
    {
        app.setStyleSheet(styleFile.readAll());
    }

    qRegisterMetaType<OperateFriendResponse>("OperateFriendResponse");
    qRegisterMetaType<FriendListResponse>("FriendListResponse");
    qRegisterMetaType<GroupingResponse>("GroupingResponse");
    qRegisterMetaType<TextRequest>("TextRequest");
    qRegisterMetaType<UserStateResponse>("UserStateResponse");
    qRegisterMetaType<GroupingFriendResponse>("GroupingFriendResponse");
    qRegisterMetaType<MsgOperateResponse>("MsgOperateResponse");
    qRegisterMetaType<TextReply>("TextReply");
    qRegisterMetaType<SimpleFileItemRequest>("SimpleFileItemRequest");
    qRegisterMetaType<TextUnit::ChatInfoUnitList>("TextUnit::ChatInfoUnitList");
    qRegisterMetaType<ResponseLogin>("ResponseLogin");
    qRegisterMetaType<LoginResponse>("LoginResponse");
    qRegisterMetaType<RegistResponse>("RegistResponse");
    qRegisterMetaType<ResponseRegister>("ResponseRegister");
    qRegisterMetaType<ResponseUpdateUser>("ResponseUpdateUser");
    qRegisterMetaType<UpdateBaseInfoResponse>("UpdateBaseInfoResponse");
    qRegisterMetaType<ResponseAddFriend>("ResponseAddFriend");
    qRegisterMetaType<SearchFriendResponse>("SearchFriendResponse");
    qRegisterMetaType<ResponseAddFriend>("ResponseAddFriend");
    qRegisterMetaType<FileItemRequest>("FileItemRequest");
    qRegisterMetaType<SimpleFileItemRequest>("SimpleFileItemRequest");
    qRegisterMetaType<RegistGroupResponse>("RegistGroupResponse");
    qRegisterMetaType<SearchGroupResponse>("SearchGroupResponse");
    qRegisterMetaType<GroupingChatResponse>("GroupingChatResponse");
    qRegisterMetaType<GroupingCommandResponse>("GroupingCommandResponse");

    RSingleton<TaskManager>::instance()->initTask();

    LoginDialog dialog;
    dialog.show();

    return app.exec();
}
