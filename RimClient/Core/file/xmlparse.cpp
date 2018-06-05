#include "xmlparse.h"

#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QDebug>

#include "Util/rlog.h"
#include "datastruct.h"

XMLParse::XMLParse(QObject *parent):QObject(parent)
{

}

bool XMLParse::parse(const QString &fileName,ParameterSettings::ParaSettings * paraSettings)
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
    QDomDocument document("document");
    if(!document.setContent(file.readAll())){
        RLOG_ERROR("Read configure file error!");
        file.close();
        return false;
    }

    QDomElement rootDom = document.documentElement();
    QDomNodeList rootChildNodes = rootDom.childNodes();
    for(int i = 0; i < rootChildNodes.size(); i++){
        QDomElement childEle = rootChildNodes.at(i).toElement();
        if(childEle.nodeName() == QStringLiteral("基本信息")){
            QDomNodeList nodeIds = childEle.elementsByTagName(QStringLiteral("本节点号"));
            if(nodeIds.size() == 1)
                paraSettings->baseInfo.nodeId = nodeIds.at(0).toElement().text();

            QDomNodeList ips = childEle.elementsByTagName(QStringLiteral("本机IP地址"));
            if(ips.size() == 1)
                paraSettings->baseInfo.localIp = ips.at(0).toElement().text();

            QDomNodeList lons = childEle.elementsByTagName(QStringLiteral("本节点经度"));
            if(lons.size() == 1)
                paraSettings->baseInfo.lon = lons.at(0).toElement().text();

            QDomNodeList lats = childEle.elementsByTagName(QStringLiteral("本节点纬度"));
            if(lats.size() == 1)
                paraSettings->baseInfo.lat = lats.at(0).toElement().text();

        }else if(childEle.nodeName() == QStringLiteral("外发信息配置")){
           QDomNodeList outerMessageNodes = childEle.childNodes();
           for(int j = 0; j < outerMessageNodes.size();j++){
                QDomElement outerEle = outerMessageNodes.at(j).toElement();
                if(!outerEle.isNull()){
                    ParameterSettings::OuterNetConfig conf;
                    conf.nodeId = outerEle.attribute(QStringLiteral("节点号"));
                    conf.channel = outerEle.attribute(QStringLiteral("通道"));
                    conf.communicationMethod = outerEle.attribute(QStringLiteral("通信方式"));
                    conf.messageFormat = outerEle.attribute(QStringLiteral("报文格式"));
                    conf.distributeMessageType = outerEle.attribute(QStringLiteral("下发报文类别"));
                    paraSettings->outerNetConfig.push_back(conf);
                }
           }
        }
    }
    return true;
}

XMLParse::~XMLParse()
{

}
