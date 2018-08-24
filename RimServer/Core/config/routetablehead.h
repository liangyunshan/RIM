#ifndef ROUTETABLEHEAD_H
#define ROUTETABLEHEAD_H

/*!
 *  @brief 视图类型
 */
enum ViewType
{
    V_SERVER,
    V_CLIENT
};

/*!
 *  @brief 客户端显示列
 */
enum TreeClientColumn{
    C_C_NODE,
    C_C_COMMUNCATE,
    C_C_FORMAT,
    C_C_CHANNEL,
    C_C_MESSTYPE,
    C_C_TOTAL_COLUMN
};

/*!
 *  @brief 服务器端显示列
 *  @details
 */
enum TreeServerColumn{
    C_S_NODE,
    C_S_IP,
    C_S_PORT,
    C_S_TOTAL_COLUMN
};

#endif // ROUTETABLEHEAD_H
