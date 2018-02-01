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
#include <QList>

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
 *     'status' :   value,                          //若是操作性质,返回操作成功与否标志,部分不会返回data段
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
    MSG_USER_VIEW_INFO,                                //用户基本信息[OK]
    MSG_USER_UPDATE_INFO,                              //用户更新信息[OK]
    MSG_USER_STATE,                                    //用户状态

    MSG_RELATION_SEARCH = 0x11,                        //查找好友[OK]
    MSG_REALTION_ADD,                                  //添加好友[OK]
    MSG_RELATION_OPERATE,                              //好友操作(参照OperateFriend)FRIEND_APPLY[OK]
    MSG_RELATION_VIEW_INFO,                            //查看好友信息
    MSG_RELATION_EDIT_INFO,                            //更新好友备注
    MSG_RELATION_LIST,                                 //请好友列表/更新好友列表

    MSG_GROUP_SEARCH = 0x31,                           //查找群
    MSG_GROUP_CREATE,                                  //创建群
    MSG_GROUP_JOIN,                                    //加入群
    MSG_GROUP_EXIT,                                    //退出群
    MSG_GROUP_REMOVE,                                  //移除群
    MSG_GROUP_DISSOLUTION,                             //解散群
    MSG_GROUP_VIEW_INFO,                               //查看群信息
    MSG_GROUP_UPDATE_INFO,                             //更新群信息

    MSG_GROUPING_OPERATE = 0x51,                       //联系人、群分组操作，操作类型见OperateGrouping

    MSG_OTHER_HEAT = 0x41                              //心跳报
};

//通用json操作结果status
enum MsgOperateResponse
{
    STATUS_SUCCESS,
    STATUS_FAILE
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

/*!
    举例：
    [1]:A 登陆信息  服务器;
    [2]:服务器 验证并发回结果信息 A
    [3]:服务器 查找历史聊天信息、请求信息 A
*/
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

/*!
    举例：小明想注册
    [1]:A 发送ResponseRegister 服务器;
    [2]:服务器 发送账号   A
*/

//注册结果信息
enum ResponseRegister
{
    REGISTER_SUCCESS = 0,           //注册成功
    REGISTER_FAILED,                //注册失败
    REGISTER_SERVER_REFUSED         //服务器未响应
};

/*!
   举例：A想添加B为好友:
   [1]:A 发送MSG_RELATION_SEARCH  服务器;         查询好友信息
   [2]:服务器 发送ResponseAddFriend  A;           查询结果
   [3]:A 发送 MSG_REALTION_ADD 服务器;            好友请求
   [4]:服务器 发送ResponseAddFriend A;            好友请求确认
   [5]:服务器 发送MSG_RELATION_REQUEST B;         好友请求(B在线直接发送；B未在线，缓存请求等B登陆)
   [6]:B 发送OperateFriend 服务器;                请求结果
   [7]:服务器 发送MSG_RELATION_REQUEST A;         转发请求结果
*/

//好友操作类型
enum OperateFriend
{
    FRIEND_APPLY,                   //好友请求
    FRIEND_DELETE,                  //删除好友
    FRIEND_MOVE                     //移动好友
};

//好友请求结果
enum ResponseFriendApply
{
    FRIEND_REQUEST,                 //请求
    FRIEND_AGREE,                   //同意请求
    FRIEND_REFUSE                   //拒绝请求
};

//删除好友结果信息
enum ResponseDeleteFriend
{
    DELETE_SUCCESS,                 //删除成功
    DELETE_FAILED                   //删除失败
};

//添加好友结果信息
enum ResponseAddFriend
{
    FIND_FRIEND_NOT_FOUND,          //未找到好友
    FIND_FRIEND_FOUND,              //找到好友
    FIND_FRIEND_FAILED,             //查找失败
    ADD_FRIEND_SENDED,              //请求发送成功
    ADD_FRIEND_SENDED_FAILED        //请求发送失败
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

/*!
    举例：小明想更新信息
    [1]:A 修改基本信息 服务器
    [2]:服务器 修改结果  A
*/
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
    QString uuid;                           //数据库数据ID
    QString accountId;                      //账号
    QString nickName;                       //昵称
    QString signName;                       //签名
    Sexual  sexual;                         //性别
    QString birthday;                       //生日
    QString address;                        //地址
    QString email;                          //邮箱
    QString phoneNumber;                    //电话
    QString remark;                         //备注
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

/***********************查询好友**********************/
struct SimpleUserInfo
{
    QString accountId;                      //账号
    QString nickName;                       //昵称
    QString signName;                       //签名
    unsigned short face;                    //头像信息(0表示为自定义，大于0表示系统头像)
    QString customImgId;                    //头像信息(face为0时有效)
    QString remarks;                        //备注名
};

enum SearchType
{
    SearchPerson,
    SearchGroup
};

class SearchFriendRequest : public MsgPacket
{
public:
    SearchFriendRequest();
    SearchType stype;                       //查询方式
    QString accountOrNickName;              //用户的AccountId或NickName
};

class SearchFriendResponse : public MsgPacket
{
public:
    SearchFriendResponse();

    QList<SimpleUserInfo> result;            //在status为FIND_FRIEND_FOUND字段时，填充查找的结果
};

/***********************添加好友**********************/
class AddFriendRequest : public MsgPacket
{
public:
    AddFriendRequest();
    SearchType stype;                        //添加人或群
    QString accountId;                       //自己id
    QString operateId;                       //想添加的用户或群ID
};

class AddFriendResponse : public MsgPacket
{
public:
    AddFriendResponse();
};

/***********************联系人操作**********************/
class OperateFriendRequest : public MsgPacket
{
public:
    OperateFriendRequest();
    OperateFriend type;
    SearchType stype;                        //添加人或群
    int result;                              //type为FRIEND_APPLY时，对应ResponseFriendApply；type为FRIEND_DELETE，对应ResponseDeleteFriend
    QString accountId;
    QString operateId;                       //待操作的好友ID。type为请求时，代表请求方ID；type为删除时，代表待删除ID
};

class OperateFriendResponse : public MsgPacket
{
public:
    OperateFriendResponse();
    OperateFriend type;
    SearchType stype;                        //添加人或群
    int result;
    QString accountId;
    SimpleUserInfo requestInfo;
};

/***********************联系人列表操作**********************/
struct RGroupData
{
    QString groupId;
    QString groupName;
    bool isDefault;
    QList<SimpleUserInfo> users;
};

class FriendListRequest : public MsgPacket
{
public:
    FriendListRequest();
    QString accountId;
};

class FriendListResponse : public MsgPacket
{
public:
    FriendListResponse();
    QString accountId;
    QList<RGroupData *> groups;
};

/**********************分组操作**********************/
//分组类型
enum GroupingType
{
    GROUPING_FRIEND,                //联系人分组
    GROUPING_GROUP                  //群分组
};

//对于联系人/群分组操作种类
enum OperateGrouping
{
    GROUPING_CREATE,                //创建分组
    GROUPING_RENAME,                //重命名分组
    GROUPING_DELETE,                //删除分组
    GROUPING_SORT,                  //分组排序
    GROUPING_REFRESH                //刷新分组

};

class GroupingRequest : public MsgPacket
{
public:
    GroupingRequest();
    QString uuid;                           //联系人uuid
    QString groupId;                        //分组ID(创建分组时无需填写)
    GroupingType gtype;                     //分组类型
    OperateGrouping type;                   //分组操作类型
    QString groupName;                      //分组名称

    //int groupIndex;                       //分组序号
};

class GroupingResponse : public MsgPacket
{
public:
    GroupingResponse();
    QString uuid;                           //联系人uuid
    GroupingType gtype;                     //分组类型
    QString groupId;                        //分组ID
    OperateGrouping type;                   //分组操作类型
};

/*********************聊天信息操作**********************/
class TextRequest : public MsgPacket
{
public:
    TextRequest();

    QString accountId;                      //用户自己ID
    SearchType type;                        //联系人or群消息
    QString destAccountId;                  //对方ID
    qint64 timeStamp;                       //时间戳
    QString sendData;                       //发送数据
};

}

#endif // PROTOCOLDATA_H
