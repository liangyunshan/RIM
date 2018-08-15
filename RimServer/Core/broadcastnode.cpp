#include "broadcastnode.h"

#include "Util/scaleswitcher.h"

BroadcastNode::BroadcastNode()
{
    defaultNode = "0xFFFF";
}

/*!
 * @brief 将本地读取的字符串转换保存
 * @param[in] list 本地广播地址
 */
void BroadcastNode::parseData(QString str)
{
    QStringList list = str.split("_");

    QList<QString>::iterator beg = list.begin();
    while(beg != list.end()){
        unsigned short tmp = ScaleSwitcher::fromHexToDec(*beg);
        if(tmp > 0)
            nodeList.push_back(tmp);
        beg++;
    }
}

/*!
 * @brief 配置列表中是否包含指定的节点，若包含则进行广播发送
 * @param[in] nodeId 待匹配的节点信息
 * @return 是否包含结果
 */
bool BroadcastNode::executeMatch(unsigned short nodeId)
{
    if(nodeList.size() == 0 || nodeId <= 0)
        return false;

    auto findIndex = std::find_if(nodeList.begin(),nodeList.end(),[&nodeId](const unsigned short node){
        return nodeId == node;
    });

    return findIndex != nodeList.end();
}
