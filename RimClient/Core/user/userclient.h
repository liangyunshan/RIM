/*!
 *  @brief     用户联系人
 *  @details   代表当前一个联系人，可通过此对象获取所有用户相关的窗口、数据等
 *  @file      userclient.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.30
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef USERCLIENT_H
#define USERCLIENT_H

#include <QHash>
#include <QString>
#include <functional>
#include <mutex>
#include <memory>

class ToolItem;
class FileDesc;
class ChatPersonWidget;

#include "../protocol/protocoldata.h"
#include "../protocol/datastruct.h"
using namespace ProtocolType;

class UserClient
{
public:
    explicit UserClient();
    ~UserClient();

    void procRecvContent(TextRequest &response);
    void procRecvServerTextReply(TextReply &reply);

    void procDownOverFile(std::shared_ptr<FileDesc> fileDesc);
    void procDownItemIcon(std::shared_ptr<FileDesc> fileDesc);
    void procTransFile(FileTransProgress);

public:
    SimpleUserInfo simpleUserInfo;
    UserBaseInfo baseInfo;
    ToolItem * toolItem;

#ifdef __LOCAL_CONTACT__
    ParameterSettings::OuterNetConfig netConfig;
#endif
    ChatPersonWidget *chatPersonWidget;
};

class UserManager
{
public:
    explicit UserManager();

    UserClient * addClient(QString accountId);
    bool removeClient(QString accountId);

    UserClient * client(ToolItem * item);
    UserClient * client(QString accountId);

    void for_each(std::function<void(UserClient *)> func);

    void closeAllClientWindow();
    int size();

private:
    QHash<QString,UserClient*> clients;
    std::mutex clientMutex;
};

#endif // USERCLIENT_H
