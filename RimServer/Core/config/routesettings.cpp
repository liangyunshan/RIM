#include "routesettings.h"

#include <functional>
#include <QDebug>

RouteSettings::RouteSettings():rsettings(NULL)
{

}

RouteSettings::~RouteSettings()
{
    release();
}

RouteSettings::RouteSettings(const RouteSettings &origin)
{
    release();
    copy(origin.rsettings);
}

RouteSettings &RouteSettings::operator=(const RouteSettings &origin)
{
    release();
    copy(origin.rsettings);
    return *this;
}

void RouteSettings::initSettings(ParameterSettings::RouteSettings *setts)
{
    if(!setts)
        return;

    release();
    copy(setts);
}

ParameterSettings::RouteSettings *RouteSettings::settings()
{
    return rsettings;
}

void RouteSettings::release()
{
    if(rsettings){
        QMap<ushort,ParameterSettings::NodeServer *>::iterator start = rsettings->servers.begin();
        while(start != rsettings->servers.end()){
            delete start.value();
            start++;
        }
        rsettings->servers.clear();

        QVector<ParameterSettings::NodeClient *>::iterator iter = rsettings->clients.begin();
        while(iter != rsettings->clients.end()){
            delete (*iter);
            iter++;
        }
        rsettings->clients.clear();

        delete rsettings;
        rsettings = NULL;
    }
}

void RouteSettings::copy(ParameterSettings::RouteSettings *setts)
{
    if(rsettings == NULL){
        rsettings = new ParameterSettings::RouteSettings;

        rsettings->baseInfo = setts->baseInfo;

        QMap<ushort,ParameterSettings::NodeServer *>::iterator start = setts->servers.begin();
        while(start != setts->servers.end()){
            ParameterSettings::NodeServer * tmpServer = start.value();

            ParameterSettings::NodeServer * ss = new ParameterSettings::NodeServer;
            ss->localIp = tmpServer->localIp;
            ss->nodeId = tmpServer->nodeId;
            ss->localPort = tmpServer->localPort;
            ss->communicationMethod = tmpServer->communicationMethod;
            ss->messageFormat = tmpServer->messageFormat;

            rsettings->servers.insert(start.key(),ss);
            start++;
        }

        QVector<ParameterSettings::NodeClient *>::iterator iter = setts->clients.begin();
        while(iter != setts->clients.end()){
            ParameterSettings::NodeClient * tmpClient = (*iter);

            ParameterSettings::NodeClient * cl = new ParameterSettings::NodeClient;
            cl->nodeId = tmpClient->nodeId;
            cl->serverNodeId = tmpClient->serverNodeId;
            cl->channel = tmpClient->channel;
            cl->communicationMethod = tmpClient->communicationMethod;
            cl->messageFormat = tmpClient->messageFormat;
            cl->distributeMessageType = tmpClient->distributeMessageType;

            ParameterSettings::NodeServer * ss = rsettings->servers.value(cl->serverNodeId);
            if(ss){
                cl->server = ss;
                ss->clients.push_back(cl);
            }
            rsettings->clients.append(cl);
            iter++;
        }
    }
}
