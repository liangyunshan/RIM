/*!
 *  @brief     通信协议内容
 *  @details   【1】定义客户端与服务器端通信请求/回复结构体，请求以XXRequest定义，回复以XXResponse定义，
 *             【2】客户端和服务器端的协议文件保持一致!!
 *  @file      protocoldata.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.15
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef PROTOCOLDATA_H
#define PROTOCOLDATA_H

#include <QString>

namespace ProtocolType {

/*!
 *1.请求头包含
 *2.Request请求信息内容(JSON数据格式)
 * {
 *    'type'    :   MsgType,
 *    'cmd'     :   MsgCommand,
 *    'data'    :
 *                   {
 *                      'key':'value'
 *                   }
 * }
 *
 *3.Response响应信息内容
 * {
 *     'type'   :   MsgType,
 *     'cmd'    :   MsgCommand,
 *     'status' :   value,                          //若是操作性质,返回操作成功与否标志
 *     'data'   :
 *                  {
 *
 *                      'key'   :  value
 *                  }
 *
 * }
 *
 *  unsigned int  totalIndex;       //总帧数量
 *  unsigned int  currIndex;        //当前帧索引
 *  unsigned int  currPackLength;   //当前包长度
 *
 */

//客户端类型(可用于以后扩展)
enum ClientType
{
    CLIENT_TYPE_UNKONWN,
    CLIENT_TYPE_PC,                    //客户端之间通信
};

/*!
 *  @brief  消息类型
 */
enum MsgType
{
    MSG_TYPE_INVALID,                   //无意义
    MSG_CONTROL,                        //控制命令
    MSG_TEXT,                           //文本信息
    MSG_IMAGE,                          //图片信息
    MSG_FILE                            //文件信息
};

/*!
 *  @brief  命令类型
 *  @note   在【MsgType为Msg_Control】时有意义
 */
enum MsgCommand
{
    MSG_Command_INVALID,                               //无意义
    MSG_USER_REGISTER = 0x01,                          //用户注册[OK]
    MSG_USER_LOGIN,                                    //用户登录[OK]
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

//性别
enum Sexual
{
    MAN,
    WOMAN
};

//在线状态
enum OnlineStatus
{
    STATUS_OFFLINE,                 //离线
    STATUS_ONLINE,                  //在线
    STATUS_INVISIBLE,               //隐身
    STATUS_BUSY,                    //忙碌
    STATUS_AWAY                     //离开
};

//登陆结果
enum ResponseLogin
{
    LOGIN_SUCCESS = 0,              //登陆成功
    LOGIN_FAILED,                   //登陆失败
    LOGIN_UNREGISTERED,             //未注册
    LOGIN_PASS_ERROR,               //密码错误
    LOGIN_USER_CANCEL_LOGIN,        //取消登陆
    LOGIN_SERVER_REFUSED,           //服务器拒绝响应
    LOGIN_SERVER_NOT_RESP           //服务器未响应
};

//注册结果信息
enum ResponseRegister
{
    REGISTER_SUCCESS = 0,           //注册成功
    REGISTER_FAILED,                //注册失败
    REGISTER_SERVER_REFUSED         //服务器未响应
};

//好友操作类型
enum OperateFriend
{
    FRIEND_APPLY,                   //好友请求
    FRIEND_AGREE,                   //同意请求
    FRIEND_REFUSE,                  //拒绝请求
    FRIEND_DELETE,                  //删除好友
};

//添加好友结果信息
enum ResponseAddFriend
{
    FIND_FRIEND_NOT_FOUND,          //未找到好友
    FIND_FRIEND_FOUND,              //找到好友
    FIND_FRIEND_FAILED,             //查找失败

    ADD_FRIEND_SUCCESS,             //添加成功
    ADD_FRIEND_FAILED               //添加失败
};

//删除好友结果信息
enum ResponseDeleteFriend
{
    DELETE_SUCCESS,                 //删除成功
    DELETE_FAILED                   //删除失败
};

//发送/接收信息结果
enum ResponseOpoerateMsg
{
    SEND_MSG_SUCCESS,               //发送消息成功
    SEND_TEXT_MSG_FAILED,           //发送文本信息失败
    SEND_IMAGE_MSG_FAILED,          //发送图片信息失败
    SEND_FILE_MSG_FAILED,           //发送文件信息失败

    RECV_FILE_SUCCESS,              //接收文件成功
    RECV_FILE_FAILED,               //接收文件失败
};

//更新用户信息结果
enum ResponseUpdateUser
{
    UPDATE_USER_SUCCESS,            //信息更新成功
    UPDATE_USER_FAILED              //信息更新失败
};

//网络协议头
class MsgPacket
{
public:
    MsgPacket();

    MsgType msgType;                //数据类型(MSG_TYPE)
    MsgCommand msgCommand;          //命令类型(MSG_COMMAND)
};

/************************登  陆**********************/
struct UserBaseInfo
{
    QString accountId;                      //账号
    QString nickName;                       //昵称
    QString signName;                       //签名
    Sexual  sexual;                         //性别
    QString birthday;                       //生日
    QString address;                        //地址
    QString email;                          //邮箱
    QString phoneNumber;                    //电话
    QString desc;                           //备注
    unsigned short face;                    //头像信息(0表示为自定义，大于0表示系统头像)
    QString customImgId;                    //头像信息(face为0时有效)
};

class LoginRequest : public MsgPacket
{
public:
    LoginRequest();

    QString accountId;
    QString password;
    OnlineStatus  status;
};

class LoginResponse : public MsgPacket
{
public:
    LoginResponse();

    UserBaseInfo baseInfo;
};

/************************注  册**********************/
class RegistRequest : public MsgPacket
{
public:
    RegistRequest();

    QString nickName;
    QString password;
};

class RegistResponse : public MsgPacket
{
public:
    RegistResponse();

    QString accountId;                       //注册的ID，在登陆时使用此ID作为用户名
};

/************************更新基本信息**********************/
class UpdateBaseInfoRequest : public MsgPacket
{
public:
    UpdateBaseInfoRequest();

    UserBaseInfo baseInfo;
};

class UpdateBaseInfoResponse : public MsgPacket
{
public:
    UpdateBaseInfoResponse();

    UserBaseInfo baseInfo;
};


}


#endif // PROTOCOLDATA_H
