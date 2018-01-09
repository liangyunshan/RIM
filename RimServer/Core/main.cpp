#include "widget.h"
#include <QApplication>

#include <QDebug>
#include <QMessageBox>
#include <QSettings>

#include "Util/rutil.h"
#include "Util/rlog.h"
#include "rsingleton.h"
#include "Network/tcpsocket.h"
#include "constants.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString configFullPath = qApp->applicationDirPath() + QString(Constant::PATH_ConfigPath);

    qDebug()<<configFullPath;

    QSettings * settings = new QSettings(configFullPath+"/config.ini",QSettings::IniFormat);
    RUtil::setGlobalSettings(settings);

    if(!RSingleton<RLog>::instance()->init())
    {
        QMessageBox::warning(NULL,QObject::tr("Warning"),QObject::tr("Log module initialization failure!"),QMessageBox::Yes,QMessageBox::Yes);
    }

    TcpSocket socket;
    if(socket.createSocket())
    {
        socket.bind("192.168.0.147",8023);
        socket.listen();
        socket.accept();
    }


    return a.exec();
}
