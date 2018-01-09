#include "msgreceive.h"

#include <QUdpSocket>

#include "Util/rlog.h"
#include "Util/rutil.h"

#include "netglobal.h"

MsgReceive::MsgReceive(QObject *parent) : QObject(parent)
{

}

MsgReceive::~MsgReceive()
{

}

