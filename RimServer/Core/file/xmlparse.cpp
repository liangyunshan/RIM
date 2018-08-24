#include "xmlparse.h"

#ifdef __LOCAL_CONTACT__

#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QTextCodec>
#include <QDebug>

#include "Util/rlog.h"
#include "Util/scaleswitcher.h"
#include "protocol/datastruct.h"

class RouteDom{
public:
    explicit RouteDom():rootNode("Route"),baseInfoNode("Baseinfo"),
    serverNode("Server"),clientNode("Client"),nodeId("nodeId"),ip("ip"),
    lon("lon"),lat("lat"),node("node"),port("port"),commethod("commethod"),messageFormat("messageFormat"),
    server("server"),channel("channel"),method("method"),format("format"),type("type"){

    }

    QString rootNode;
    QString baseInfoNode;
    QString serverNode;
    QString clientNode;

    QString nodeId;
    QString ip;
    QString lon;
    QString lat;

    QString node;
    QString port;
    QString commethod;
    QString messageFormat;
    QString server;
    QString channel;
    QString method;
    QString format;
    QString type;
};

XMLParse::XMLParse(QObject *parent):QObject(parent)
{

}

/*!
 * @brief 解析参数设置信息
 * @param[in] fileName 待解析的文件名
 * @param[in] routeSettings 解析后保存的结构体
 * @return 是否解析成功
 */
bool XMLParse::parseParaSettings(const QString &fileName,ParameterSettings::ParaSettings * paraSettings)
{
    QDomDocument document("document");
    if(!validateParseFile(fileName,document))
        return false;

    QDomElement rootDom = document.documentElement();
    QDomNodeList rootChildNodes = rootDom.childNodes();
    for(int i = 0; i < rootChildNodes.size(); i++){
        QDomElement childEle = rootChildNodes.at(i).toElement();
        if(childEle.nodeName() == QStringLiteral("基本信息")){
            QDomNodeList nodeIds = childEle.elementsByTagName(QStringLiteral("本节点号"));
            if(nodeIds.size() == 1)
                paraSettings->baseInfo.nodeId = ScaleSwitcher::fromHexToDec(nodeIds.at(0).toElement().text());

            QDomNodeList ips = childEle.elementsByTagName(QStringLiteral("本机IP地址"));
            if(ips.size() == 1)
                paraSettings->baseInfo.localIp = ips.at(0).toElement().text();

            QDomNodeList lons = childEle.elementsByTagName(QStringLiteral("本节点经度"));
            if(lons.size() == 1)
                paraSettings->baseInfo.lon = lons.at(0).toElement().text().toDouble();

            QDomNodeList lats = childEle.elementsByTagName(QStringLiteral("本节点纬度"));
            if(lats.size() == 1)
                paraSettings->baseInfo.lat = lats.at(0).toElement().text().toDouble();

        }else if(childEle.nodeName() == QStringLiteral("外发信息配置")){
           QDomNodeList outerMessageNodes = childEle.childNodes();
           for(int j = 0; j < outerMessageNodes.size();j++){
                QDomElement outerEle = outerMessageNodes.at(j).toElement();
                if(!outerEle.isNull()){
                    ParameterSettings::OuterNetConfig conf;
                    conf.nodeId = ScaleSwitcher::fromHexToDec(outerEle.attribute(QStringLiteral("节点号")));
                    conf.channel = outerEle.attribute(QStringLiteral("通道"));
                    conf.communicationMethod = static_cast<ParameterSettings::CommucationMethod>(outerEle.attribute(QStringLiteral("通信方式")).toInt());
                    conf.messageFormat = static_cast<ParameterSettings::MessageFormat>(outerEle.attribute(QStringLiteral("报文格式")).toInt());
                    conf.distributeMessageType = outerEle.attribute(QStringLiteral("下发报文类别"));
                    paraSettings->outerNetConfig.push_back(conf);
                }
           }
        }else if(childEle.nodeName() == QStringLiteral("通信控制器")){
           QDomNodeList ips = childEle.elementsByTagName(QStringLiteral("通控器IP"));
           if(ips.size() == 1)
               paraSettings->commControl.ip = ips.at(0).toElement().text();

           QDomNodeList ports = childEle.elementsByTagName(QStringLiteral("通控器端口"));
           if(ports.size() == 1)
               paraSettings->commControl.port = ports.at(0).toElement().text().toUShort();

           QDomNodeList emulateIps = childEle.elementsByTagName(QStringLiteral("模拟器IP"));
           if(emulateIps.size() == 1)
               paraSettings->commControl.emualteIp = emulateIps.at(0).toElement().text();

           QDomNodeList emulatePorts = childEle.elementsByTagName(QStringLiteral("模拟器端口"));
           if(emulatePorts.size() == 1)
               paraSettings->commControl.emulatePort = emulatePorts.at(0).toElement().text().toUShort();

           QDomNodeList transmitSettings = childEle.elementsByTagName(QStringLiteral("传输配置"));
           if(transmitSettings.size() == 1){
                QDomNodeList childNodes = transmitSettings.at(0).toElement().childNodes();
                for(int i = 0 ; i < childNodes.size();i++){
                    QDomElement node = childNodes.at(i).toElement();
                    if(!node.isNull()){
                        ParameterSettings::TrasmitSetting settigs;
                        settigs.nodeId = node.attribute(QStringLiteral("节点号"));
                        settigs.protocol = node.attribute(QStringLiteral("传输协议"));
                        settigs.messageFormat = static_cast<ParameterSettings::MessageFormat>(node.attribute(QStringLiteral("报文格式")).toInt());
                        paraSettings->commControl.transmitSettings.append(settigs);
                    }
                }
           }
        }else if(childEle.nodeName() == QStringLiteral("网络收发配置")){
            QDomNodeList netParamSettings = childEle.elementsByTagName(QStringLiteral("网络参数"));
            if(netParamSettings.size() == 1){
                QDomElement netParamEle = netParamSettings.at(0).toElement();

                QDomNodeList ports = netParamEle.elementsByTagName(QStringLiteral("收发端口"));
                if(ports.size() == 1)
                    paraSettings->netSites.netParamSetting.port = ports.at(0).toElement().text().toUShort();

                QDomNodeList backPorts = netParamEle.elementsByTagName(QStringLiteral("收发辅助端口"));
                if(backPorts.size() == 1)
                    paraSettings->netSites.netParamSetting.backPort = backPorts.at(0).toElement().text().toUShort();

                QDomNodeList maxSegments = netParamEle.elementsByTagName(QStringLiteral("发送报文最大长度"));
                if(maxSegments.size() == 1)
                    paraSettings->netSites.netParamSetting.maxSegment = maxSegments.at(0).toElement().text().toUShort();

                QDomNodeList delayTimes = netParamEle.elementsByTagName(QStringLiteral("网络发送时延"));
                if(delayTimes.size() == 1)
                    paraSettings->netSites.netParamSetting.delayTime = delayTimes.at(0).toElement().text().toUShort();
            }

            QDomNodeList sites = childEle.elementsByTagName(QStringLiteral("网络信源"));
            if(sites.size() == 1){
                QDomElement siteElement = sites.at(0).toElement();

                QDomNodeList siteCollects = siteElement.elementsByTagName(QStringLiteral("信源"));
                for(int k = 0; k < siteCollects.size();k++){
                    QDomElement node = siteCollects.at(k).toElement();

                    ParameterSettings::MessSource messSource;

                    QDomNodeList nodes = node.elementsByTagName(QStringLiteral("节点名"));
                    if(nodes.size() == 1)
                        messSource.nodeName = nodes.at(0).toElement().text();

                    QDomNodeList nodeIds = node.elementsByTagName(QStringLiteral("节点号"));
                    if(nodeIds.size() == 1)
                        messSource.nodeId = ScaleSwitcher::fromHexToDec(nodeIds.at(0).toElement().text());

                    QDomNodeList ips = node.elementsByTagName(QStringLiteral("IP地址"));
                    if(ips.size() == 1)
                        messSource.ip = ips.at(0).toElement().text();

                    QDomNodeList encryptions = node.elementsByTagName(QStringLiteral("加密标识"));
                    if(encryptions.size() == 1)
                        messSource.encryption = encryptions.at(0).toElement().text().toUShort();

                    QDomNodeList prioritys = node.elementsByTagName(QStringLiteral("发送优先级"));
                    if(prioritys.size() == 1)
                        messSource.priority = prioritys.at(0).toElement().text().toUShort();

                    QDomNodeList connectTimeOuts = node.elementsByTagName(QStringLiteral("建链初始时延"));
                    if(connectTimeOuts.size() == 1)
                        messSource.connectTimeOut = connectTimeOuts.at(0).toElement().text().toUShort();

                    QDomNodeList protocols = node.elementsByTagName(QStringLiteral("传输协议"));
                    if(protocols.size() == 1)
                        messSource.protocol = protocols.at(0).toElement().text();

                    QDomNodeList ports = node.elementsByTagName(QStringLiteral("端口号"));
                    if(ports.size() == 1)
                        messSource.port = ports.at(0).toElement().text().toUShort();

                    paraSettings->netSites.sites.append(messSource);
                }
            }
        }
    }
    return true;
}

/*!
 * @brief 解析路由表信息
 * @param[in] fileName 待解析的文件名
 * @param[in] routeSettings 解析后保存的结构体
 * @return 是否解析成功
 */
bool XMLParse::parseRouteSettings(const QString &fileName, ParameterSettings::RouteSettings *routeSettings)
{
    QDomDocument document("document");
    if(!validateParseFile(fileName,document))
        return false;

    RouteDom routeDom;
    QDomElement rootDom = document.documentElement();

    QDomNodeList baseInfoNode = rootDom.elementsByTagName(routeDom.baseInfoNode);
    if(baseInfoNode.size() == 1){
        QDomElement baseInfoElement = baseInfoNode.at(0).toElement();

        QDomNodeList nodeIds = baseInfoElement.elementsByTagName(routeDom.nodeId);
        if(nodeIds.size() == 1)
            routeSettings->baseInfo.nodeId = ScaleSwitcher::fromHexToDec(nodeIds.at(0).toElement().text());

        QDomNodeList ips = baseInfoElement.elementsByTagName(routeDom.ip);
        if(ips.size() == 1)
            routeSettings->baseInfo.localIp = ips.at(0).toElement().text();

        QDomNodeList lons = baseInfoElement.elementsByTagName(routeDom.lon);
        if(lons.size() == 1)
            routeSettings->baseInfo.lon = lons.at(0).toElement().text().toDouble();

        QDomNodeList lats = baseInfoElement.elementsByTagName(routeDom.lat);
        if(lats.size() == 1)
            routeSettings->baseInfo.lat = lats.at(0).toElement().text().toDouble();
    }

    QDomNodeList serverNode = rootDom.elementsByTagName(routeDom.serverNode);
    if(serverNode.size() == 1){
        QDomNodeList serverNodes = serverNode.at(0).toElement().childNodes();
        for(int i = 0; i < serverNodes.count();i++){
            QDomElement server = serverNodes.at(i).toElement();
            ParameterSettings::NodeServer * ss = new ParameterSettings::NodeServer;
            ss->localIp = server.attribute(routeDom.ip);
            ss->nodeId = ScaleSwitcher::fromHexToDec(server.attribute(routeDom.nodeId));
            ss->localPort = server.attribute(routeDom.port);
            ss->communicationMethod = static_cast<ParameterSettings::CommucationMethod> (server.attribute(routeDom.commethod).toInt());
            ss->messageFormat = static_cast<ParameterSettings::MessageFormat> (server.attribute(routeDom.messageFormat).toInt());
            routeSettings->servers.insert(ss->nodeId,ss);
        }
    }

    QDomNodeList clientNode = rootDom.elementsByTagName(routeDom.clientNode);
    if(clientNode.size() == 1){
        QDomNodeList clientNodes = clientNode.at(0).toElement().childNodes();
        for(int i = 0;i < clientNodes.size();i++){
            QDomElement client = clientNodes.at(i).toElement();
            ParameterSettings::NodeClient * cl = new ParameterSettings::NodeClient;
            cl->nodeId = ScaleSwitcher::fromHexToDec(client.attribute(routeDom.nodeId));
            cl->serverNodeId = ScaleSwitcher::fromHexToDec(client.attribute(routeDom.server));

            cl->channel = client.attribute(routeDom.channel);
            cl->communicationMethod = static_cast<ParameterSettings::CommucationMethod>(client.attribute(routeDom.method).toInt());
            cl->messageFormat = static_cast<ParameterSettings::MessageFormat>(client.attribute(routeDom.format).toInt());
            cl->distributeMessageType = client.attribute(routeDom.type);

            ParameterSettings::NodeServer * ss = routeSettings->servers.value(cl->serverNodeId);
            if(ss){
                cl->server = ss;
                ss->clients.push_back(cl);
            }

            routeSettings->clients.append(cl);
        }
    }

    return true;
}

/*!
 * @brief 保存更新后的路由表
 * @param[in] fileName 待保存的文件名
 * @param[in] routeSettings 待保存的路由信息
 * @return  true保存成功，false保存失败
 */
bool XMLParse::saveRouteSettings(const QString &fileName, ParameterSettings::RouteSettings *routeSettings)
{
    RouteDom routeDom;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly)){
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForLocale());

    QDomDocument doc("Route table");

    QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml","version='1.0' encoding='UTF-8'");
    doc.appendChild(instruction);

    QDomElement root = doc.createElement(routeDom.rootNode);
    doc.appendChild(root);

    //BaseInfo
    {
        QDomElement baseInfoElement = doc.createElement(routeDom.baseInfoNode);
        QDomElement nodeId = doc.createElement(routeDom.nodeId);
        QDomText nodeText = doc.createTextNode(ScaleSwitcher::fromDecToHex(routeSettings->baseInfo.nodeId));
        nodeId.appendChild(nodeText);

        QDomElement ipId = doc.createElement(routeDom.ip);
        QDomText ipText = doc.createTextNode(routeSettings->baseInfo.localIp);
        ipId.appendChild(ipText);

        QDomElement lonId = doc.createElement(routeDom.lon);
        QDomText lonText = doc.createTextNode(QString::number(routeSettings->baseInfo.lon));
        lonId.appendChild(lonText);

        QDomElement latId = doc.createElement(routeDom.lat);
        QDomText latText = doc.createTextNode(QString::number(routeSettings->baseInfo.lat));
        latId.appendChild(latText);

        baseInfoElement.appendChild(nodeId);
        baseInfoElement.appendChild(ipId);
        baseInfoElement.appendChild(lonId);
        baseInfoElement.appendChild(latId);

        root.appendChild(baseInfoElement);
    }

    //Server
    {
        QDomElement serverElement = doc.createElement(routeDom.serverNode);
        QMap<ushort,ParameterSettings::NodeServer *>::iterator iter = routeSettings->servers.begin();
        while(iter != routeSettings->servers.end()){
            ParameterSettings::NodeServer * server = iter.value();
            QDomElement serverNode = doc.createElement(routeDom.node);
            serverNode.setAttribute(routeDom.nodeId,ScaleSwitcher::fromDecToHex(server->nodeId));
            serverNode.setAttribute(routeDom.ip,server->localIp);
            serverNode.setAttribute(routeDom.port,server->localPort);
            serverNode.setAttribute(routeDom.method,(int)server->communicationMethod);
            serverNode.setAttribute(routeDom.messageFormat,(int)server->messageFormat);

            serverElement.appendChild(serverNode);
            iter++;
        }
        root.appendChild(serverElement);
    }

    //Client
    {
        QDomElement clientElement = doc.createElement(routeDom.clientNode);
        QVector<ParameterSettings::NodeClient *>::iterator iter = routeSettings->clients.begin();
        while(iter != routeSettings->clients.end()){
            ParameterSettings::NodeClient * client = (*iter);
            QDomElement clientNode = doc.createElement(routeDom.node);
            clientNode.setAttribute(routeDom.nodeId,ScaleSwitcher::fromDecToHex(client->nodeId));
            clientNode.setAttribute(routeDom.channel,client->channel);
            clientNode.setAttribute(routeDom.method,(int)client->communicationMethod);
            clientNode.setAttribute(routeDom.format,(int)client->messageFormat);
            clientNode.setAttribute(routeDom.type,client->distributeMessageType);
            if(client->server)
                clientNode.setAttribute(routeDom.server,ScaleSwitcher::fromDecToHex(client->server->nodeId));

            clientElement.appendChild(clientNode);
            iter++;
        }
        root.appendChild(clientElement);
    }

    doc.save(stream,4);

    return true;
}

/*!
 * @brief 验证待解析的配置文件
 * @param[in] fileName 待解析的文件名
 * @return 是否验证通过
 */
bool XMLParse::validateParseFile(const QString &fileName,QDomDocument &document)
{
    QFileInfo fileInfo(fileName);
    if(!fileInfo.exists()){
        RLOG_ERROR("Configure file not existed!");
        return false;
    }

    QFile file(fileName);
    if(!file.open(QFile::ReadOnly)){
        return false;
    }

    if(!document.setContent(file.readAll())){
        RLOG_ERROR("Read configure file error!");
        file.close();
        return false;
    }

    return true;
}

XMLParse::~XMLParse()
{

}

#endif
