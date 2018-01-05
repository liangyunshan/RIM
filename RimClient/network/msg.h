/*!
 *  @brief     信息结构
 *  @details   定义与服务器端传输时所需要的各种信息
 *  @file      msg.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.04
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef MSG_H
#define MSG_H

/*!
 *  @brief  消息类型
 */
enum Msg_Type
{
    MSG_CONTROL,                        //控制命令
    MSG_TEXT,                           //文本信息
    MSG_IMAGE,                          //图片信息
    MSG_FILE                            //文件信息
};

/*!
 *  @brief  命令类型
 *  @note   在MsgType为Msg_Control时有意义
 */
enum Msg_Command
{
    MSG_USER_REGISTER = 0x01,                          //用户注册
    MSG_USER_LOGIN,                                    //用户登录
    MSG_USER_LOGOUT,                                   //用户退出
    MSG_USER_VIEW_INFO,                                //用户基本信息
    MSG_USER_UPDATE_INFO,                              //用户更新信息
    MSG_USER_STATE,                                    //用户状态

    MSG_RELATION_SEARCH = 0x11,                        //查找好友
    MSG_REALTION_ADD,                                  //添加好友
    MSG_RELATION_REMOVE,                               //删除好友
    MSG_RELATION_VIEW_INFO,                            //查看好友信息
    MSG_RELATION_EDIT_INFO,                            //更新好友备注
    MSG_RELATION_MOVE,                                 //移动好友

    MSG_GROUP_SEARCH = 0x31,                           //查找群
    MSG_GROUP_CREATE,                                  //创建群
    MSG_GROUP_JOIN,                                    //加入群
    MSG_GROUP_EXIT,                                    //退出群
    MSG_GROUP_REMOVE,                                  //移除群
    MSG_GROUP_DISSOLUTION,                             //解散群
    MSG_GROUP_VIEW_INFO,                               //查看群信息
    MSG_GROUP_UPDATE_INFO,                             //更新群信息

    MSG_OTHER_HEAT = 0x41                              //心跳报
};

/*!   消息主体
 *  {"mgic":12,"mess":1,"cmd":2,"time":3,"len":4,"content":{}}
 */

#endif // MSG_H
