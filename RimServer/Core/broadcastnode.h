/*!
 *  @brief     广播节点设置
 *  @details   用于管理用于广播的节点信息,可配置多个广播节点信息
 *  @author    wey
 *  @version   1.0
 *  @date      2018.08.14
 *  @warning
 *  @copyright NanJing RenGu.
 *  @note
 */
#ifndef BROADCASTNODE_H
#define BROADCASTNODE_H

#include <list>
#include <QStringList>

class BroadcastNode
{
public:
    BroadcastNode();

    QString getDefaultNode(){return defaultNode;}
    void parseData(QString str);

    bool executeMatch(unsigned short nodeId);

private:
    std::list<unsigned short> nodeList;
    QString defaultNode;
};

#endif // BROADCASTNODE_H
