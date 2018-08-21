#include "networksetting.h"

IpPort::IpPort()
{
    port = 0;
    connected = false;
}

IpPort::IpPort(QString ip, unsigned short port)
{
    this->ip = ip;
    this->port = port;
    connected = false;
}

IpPort::IpPort(const IpPort &otherSide)
{
    this->ip = otherSide.ip;
    this->port = otherSide.port;
    this->connected = otherSide.connected;
}

IpPort IpPort::operator=(const IpPort &otherSide)
{
    if(this != &otherSide){
        this->ip = otherSide.ip;
        this->port = otherSide.port;
        this->connected = otherSide.connected;
    }

    return *this;
}

bool IpPort::operator==(const IpPort &otherSide)
{
    if(ip != otherSide.ip || port != otherSide.port){
        return false;
    }
    return true;
}

bool IpPort::isValid()
{
    return (ip.size() > 0 && port > 0);
}

QVector<IpPort> NetworkSettings::validTextIps()
{
    QVector<IpPort> ipCollects;

    if(textServer.isValid())
        ipCollects.append(textServer);
#ifdef __LOCAL_CONTACT__
    if(tandemServer.isValid())
        ipCollects.append(tandemServer);
#endif
    return ipCollects;
}

#ifndef __LOCAL_CONTACT__
QVector<IpPort> NetworkSettings::validFileIps()
{
    QVector<IpPort> ipCollects;

    if(fileServer.isValid())
        ipCollects.append(fileServer);

    return ipCollects;
}
#endif
