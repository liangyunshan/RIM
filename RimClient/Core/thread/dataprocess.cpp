#include "dataprocess.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "Util/rlog.h"
#include "rsingleton.h"
#include "messdiapatch.h"
#include "jsonkey.h"
#include "file/filedesc.h"
#include "file/filemanager.h"

#include "protocoldata.h"
using namespace ProtocolType;

#define FILE_MAX_PACK_SIZE 900      //文件每包最大的长度

DataProcess::DataProcess()
{

}

void DataProcess::proRegistResponse(QJsonObject & data)
{
    RegistResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == REGISTER_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            MessDiapatch::instance()->onRecvRegistResponse(REGISTER_SUCCESS,response);
        }
    }
    else
    {
        ResponseRegister rr = (ResponseRegister)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvRegistResponse(rr,response);
    }
}

void DataProcess::proLoginResponse(QJsonObject &data)
{
    LoginResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == LOGIN_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.baseInfo.uuid = dataObj.value(JsonKey::key(JsonKey::Uuid)).toString();
            response.baseInfo.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            response.baseInfo.nickName = dataObj.value(JsonKey::key(JsonKey::NickName)).toString();
            response.baseInfo.signName = dataObj.value(JsonKey::key(JsonKey::SignName)).toString();
            response.baseInfo.sexual = (Sexual)dataObj.value(JsonKey::key(JsonKey::Sexual)).toInt();
            response.baseInfo.birthday = dataObj.value(JsonKey::key(JsonKey::Birth)).toString();
            response.baseInfo.address = dataObj.value(JsonKey::key(JsonKey::Address)).toString();
            response.baseInfo.email = dataObj.value(JsonKey::key(JsonKey::Email)).toString();
            response.baseInfo.phoneNumber = dataObj.value(JsonKey::key(JsonKey::Phone)).toString();
            response.baseInfo.remark = dataObj.value(JsonKey::key(JsonKey::Remark)).toString();
            response.baseInfo.isSystemIcon = dataObj.value(JsonKey::key(JsonKey::SystemIcon)).toBool();
            response.baseInfo.iconId = dataObj.value(JsonKey::key(JsonKey::IconId)).toString();

            MessDiapatch::instance()->onRecvLoginResponse(LOGIN_SUCCESS,response);
        }
    }
    else
    {
        ResponseLogin rr = (ResponseLogin)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvLoginResponse(rr,response);
    }
}

void DataProcess::proUpdateBaseInfoResponse(QJsonObject &data)
{
    UpdateBaseInfoResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == UPDATE_USER_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.baseInfo.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            response.baseInfo.nickName = dataObj.value(JsonKey::key(JsonKey::NickName)).toString();
            response.baseInfo.signName = dataObj.value(JsonKey::key(JsonKey::SignName)).toString();
            response.baseInfo.sexual = (Sexual)dataObj.value(JsonKey::key(JsonKey::Sexual)).toInt();
            response.baseInfo.birthday = dataObj.value(JsonKey::key(JsonKey::Birth)).toString();
            response.baseInfo.address = dataObj.value(JsonKey::key(JsonKey::Address)).toString();
            response.baseInfo.email = dataObj.value(JsonKey::key(JsonKey::Email)).toString();
            response.baseInfo.phoneNumber = dataObj.value(JsonKey::key(JsonKey::Phone)).toString();
            response.baseInfo.remark = dataObj.value(JsonKey::key(JsonKey::Remark)).toString();
            response.baseInfo.isSystemIcon = dataObj.value(JsonKey::key(JsonKey::SystemIcon)).toBool();
            response.baseInfo.iconId = dataObj.value(JsonKey::key(JsonKey::IconId)).toString();
            response.reponseType = (OperateContact)dataObj.value(JsonKey::key(JsonKey::Type)).toInt();
            MessDiapatch::instance()->onRecvUpdateBaseInfoResponse(UPDATE_USER_SUCCESS,response);
        }
    }
    else
    {
        ResponseUpdateUser rr = (ResponseUpdateUser)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvUpdateBaseInfoResponse(rr,response);
    }
}

void DataProcess::proUserStateChanged(QJsonObject &data)
{
    UserStateResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == STATUS_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        if(!dataObj.isEmpty())
        {
            response.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
            response.onStatus = (OnlineStatus)dataObj.value(JsonKey::key(JsonKey::Status)).toInt();

            MessDiapatch::instance()->onRecvUserStateChangedResponse(STATUS_SUCCESS,response);
        }
    }
    else
    {
        MessDiapatch::instance()->onRecvUserStateChangedResponse(STATUS_FAILE,response);
    }
}

void DataProcess::proSearchFriendResponse(QJsonObject &data)
{
    SearchFriendResponse response;
    if(data.value(JsonKey::key(JsonKey::Status)).toInt() == FIND_FRIEND_FOUND)
    {
        QJsonArray dataArray = data.value(JsonKey::key(JsonKey::Data)).toArray();
        if(!dataArray.isEmpty())
        {
            for(int i = 0; i < dataArray.count(); i++)
            {
                QJsonObject obj = dataArray.at(i).toObject();
                if(!obj.isEmpty())
                {
                    SimpleUserInfo result;
                    result.accountId = obj.value(JsonKey::key(JsonKey::AccountId)).toString();
                    result.nickName = obj.value(JsonKey::key(JsonKey::NickName)).toString();
                    result.signName = obj.value(JsonKey::key(JsonKey::SignName)).toString();
                    result.isSystemIcon = obj.value(JsonKey::key(JsonKey::SystemIcon)).toBool();
                    result.iconId = obj.value(JsonKey::key(JsonKey::IconId)).toString();

                    response.result.append(result);
                }
            }

            MessDiapatch::instance()->onRecvSearchFriendResponse(FIND_FRIEND_FOUND,response);
        }
    }
    else
    {
        ResponseAddFriend rr = (ResponseAddFriend)data.value(JsonKey::key(JsonKey::Status)).toInt();
        MessDiapatch::instance()->onRecvSearchFriendResponse(rr,response);
    }
}

void DataProcess::proAddFriendResponse(QJsonObject &data)
{
   MessDiapatch::instance()->onRecvAddFriendResponse((ResponseAddFriend)data.value(JsonKey::key(JsonKey::Status)).toInt());
}

void DataProcess::proOperateFriendResponse(QJsonObject &data)
{
    QJsonObject dataobj = data.value(JsonKey::key(JsonKey::Data)).toObject();

    OperateFriend optype = (OperateFriend)dataobj.value(JsonKey::key(JsonKey::Type)).toInt();
    switch(optype)
    {
        case FRIEND_APPLY:
                           {
                                OperateFriendResponse response;

                                response.accountId = dataobj.value(JsonKey::key(JsonKey::AccountId)).toString();
                                response.result = dataobj.value(JsonKey::key(JsonKey::Result)).toInt();
                                response.stype = (SearchType)dataobj.value(JsonKey::key(JsonKey::SearchType)).toInt();
                                QJsonObject requestInfo = dataobj.value(JsonKey::key(JsonKey::OperateInfo)).toObject();
                                if(!requestInfo.isEmpty())
                                {
                                    response.requestInfo.accountId = requestInfo.value(JsonKey::key(JsonKey::AccountId)).toString();
                                    response.requestInfo.nickName = requestInfo.value(JsonKey::key(JsonKey::NickName)).toString();
                                    response.requestInfo.signName = requestInfo.value(JsonKey::key(JsonKey::SignName)).toString();
                                    response.requestInfo.isSystemIcon = requestInfo.value(JsonKey::key(JsonKey::SystemIcon)).toInt();
                                    response.requestInfo.iconId = requestInfo.value(JsonKey::key(JsonKey::IconId)).toString();
                                }

                                MessDiapatch::instance()->onRecvFriendRequest(response);
                           }
                           break;
        default:
            break;
    }
}

void DataProcess::proFriendListResponse(QJsonObject &data)
{
    MsgOperateResponse status = (MsgOperateResponse)data.value(JsonKey::key(JsonKey::Status)).toInt();
    if(status == STATUS_SUCCESS)
    {
        QJsonObject dataObject = data.value(JsonKey::key(JsonKey::Data)).toObject();

        FriendListResponse * response = new FriendListResponse;
        response->accountId = dataObject.value(JsonKey::key(JsonKey::AccountId)).toString();
        QJsonArray groups = dataObject.value(JsonKey::key(JsonKey::Groups)).toArray();
        for(int i = 0; i < groups.size(); i++)
        {
            QJsonObject group = groups.at(i).toObject();

            RGroupData * groupData = new RGroupData;
            groupData->groupId = group.value(JsonKey::key(JsonKey::GroupId)).toString();
            groupData->groupName = group.value(JsonKey::key(JsonKey::GroupName)).toString();
            groupData->isDefault = group.value(JsonKey::key(JsonKey::IsDefault)).toBool();
            groupData->index = group.value(JsonKey::key(JsonKey::Index)).toInt();

            QJsonArray users = group.value(JsonKey::key(JsonKey::Users)).toArray();
            for(int j = 0; j < users.size();j++)
            {
                QJsonObject user = users.at(j).toObject();

                SimpleUserInfo * userInfo = new SimpleUserInfo();
                userInfo->accountId = user.value(JsonKey::key(JsonKey::AccountId)).toString();
                userInfo->nickName = user.value(JsonKey::key(JsonKey::NickName)).toString();
                userInfo->signName = user.value(JsonKey::key(JsonKey::SignName)).toString();
                userInfo->remarks = user.value(JsonKey::key(JsonKey::Remark)).toString();
                userInfo->isSystemIcon = user.value(JsonKey::key(JsonKey::SystemIcon)).toInt();
                userInfo->iconId = user.value(JsonKey::key(JsonKey::IconId)).toString();
                userInfo->status = (OnlineStatus)user.value(JsonKey::key(JsonKey::Status)).toInt();

                groupData->users.append(userInfo);
            }

            response->groups.append(groupData);
        }

        MessDiapatch::instance()->onRecvFriendList(response);
    }
    else
    {

    }
}

void DataProcess::proGroupingOperateResponse(QJsonObject &data)
{
    MsgOperateResponse status = (MsgOperateResponse)data.value(JsonKey::key(JsonKey::Status)).toInt();
    if(status == STATUS_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        GroupingResponse response;
        response.gtype = (GroupingType)dataObj.value(JsonKey::key(JsonKey::GroupType)).toInt();
        response.type = (OperateGrouping) dataObj.value(JsonKey::key(JsonKey::Type)).toInt();
        response.groupId = dataObj.value(JsonKey::key(JsonKey::GroupId)).toString();
        response.uuid = dataObj.value(JsonKey::key(JsonKey::Uuid)).toString();
        response.groupIndex = dataObj.value(JsonKey::key(JsonKey::Index)).toInt();
        if(response.gtype == GROUPING_FRIEND)
            MessDiapatch::instance()->onRecvFriendGroupingOperate(response);
        else if(response.gtype == GROUPING_GROUP)
            MessDiapatch::instance()->onRecvGroupGroupingOperate(response);
    }
    else
    {

    }
}

void DataProcess::proGroupingFriendResponse(QJsonObject &data)
{
    MsgOperateResponse status = (MsgOperateResponse)data.value(JsonKey::key(JsonKey::Status)).toInt();
    QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
    SearchType type = (SearchType)dataObj.value(JsonKey::key(JsonKey::SearchType)).toInt();
    if(type == SearchPerson)
    {
        GroupingFriendResponse response;
        response.type = (OperateGroupingFriend) dataObj.value(JsonKey::key(JsonKey::Type)).toInt();
        response.groupId = dataObj.value(JsonKey::key(JsonKey::GroupId)).toString();
        response.oldGroupId = dataObj.value(JsonKey::key(JsonKey::OldGroupId)).toString();
        response.stype = (SearchType)dataObj.value(JsonKey::key(JsonKey::SearchType)).toInt();

        QJsonObject simpleObj = dataObj.value(JsonKey::key(JsonKey::Users)).toObject();
        response.user.accountId = simpleObj.value(JsonKey::key(JsonKey::AccountId)).toString();
        response.user.nickName = simpleObj.value(JsonKey::key(JsonKey::NickName)).toString();
        response.user.signName = simpleObj.value(JsonKey::key(JsonKey::SignName)).toString();
        response.user.isSystemIcon = simpleObj.value(JsonKey::key(JsonKey::SystemIcon)).toInt();
        response.user.iconId = simpleObj.value(JsonKey::key(JsonKey::IconId)).toString();
        response.user.remarks = simpleObj.value(JsonKey::key(JsonKey::Remark)).toString();
        response.user.status = (OnlineStatus)simpleObj.value(JsonKey::key(JsonKey::Status)).toInt();

        MessDiapatch::instance()->onRecvGroupingFriend(status,response);
    }
}

void DataProcess::proText(QJsonObject &data)
{
    MsgOperateResponse result = (MsgOperateResponse)data.value(JsonKey::key(JsonKey::Status)).toInt();
    if(result == STATUS_SUCCESS)
    {
        QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
        TextRequest response;

        response.msgCommand = (MsgCommand)data.value(JsonKey::key(JsonKey::Command)).toInt();

        response.accountId = dataObj.value(JsonKey::key(JsonKey::AccountId)).toString();
        response.textId = dataObj.value(JsonKey::key(JsonKey::TextId)).toString();
        response.otherSideId = dataObj.value(JsonKey::key(JsonKey::OtherSideId)).toString();
        response.type = (SearchType)dataObj.value(JsonKey::key(JsonKey::SearchType)).toInt();
        response.timeStamp = dataObj.value(JsonKey::key(JsonKey::Time)).toVariant().toULongLong();
        response.isEncryption = dataObj.value(JsonKey::key(JsonKey::Encryption)).toBool();
        response.isCompress = dataObj.value(JsonKey::key(JsonKey::Compress)).toBool();
        response.textType = (TextType)dataObj.value(JsonKey::key(JsonKey::Type)).toInt();
        response.sendData = dataObj.value(JsonKey::key(JsonKey::Data)).toString();

        MessDiapatch::instance()->onRecvText(response);
    }
    else
    {

    }
}

void DataProcess::proTextApply(QJsonObject &data)
{
    QJsonObject dataObj = data.value(JsonKey::key(JsonKey::Data)).toObject();
    TextReply textReply;
    textReply.textId = dataObj.value(JsonKey::key(JsonKey::TextId)).toString();
    textReply.applyType = (TextReplyType)dataObj.value(JsonKey::key(JsonKey::Type)).toInt();

    MessDiapatch::instance()->onRecvTextReply(textReply);
}

/*!
 * @attention 注意RBuffer中数据的开始指针已经不是指向0的位置
 */
void DataProcess::proFileControl(RBuffer &data)
{
    SimpleFileItemRequest simpleControl;

    int status;
    if(!data.read(status))
        return;
    if((MsgOperateResponse)status == STATUS_SUCCESS)
    {
        int control;
        if(!data.read(control))
            return;
        simpleControl.control = (FileTransferControl)control;

        int itemType;
        if(!data.read(itemType))
            return;
        simpleControl.itemType = static_cast<FileItemType>(itemType);

        if(!data.read(simpleControl.md5))
            return;

        if(!data.read(simpleControl.fileId))
            return;

        MessDiapatch::instance()->onRecvFileControl(simpleControl);
    }
}

/*!
 * @attention 注意RBuffer中数据的开始指针已经不是指向0的位置
 */
void DataProcess::proFileRequest(RBuffer & data)
{
    FileItemRequest itemRequest;

    int status;
    if(!data.read(status))
        return;
    if((MsgOperateResponse)status == STATUS_SUCCESS)
    {
        int control;
        if(!data.read(control))
            return;
        itemRequest.control = (FileTransferControl)control;

        int itemType;
        if(!data.read(itemType))
            return;
        itemRequest.itemType = static_cast<FileItemType>(itemType);

        if(!data.read(itemRequest.fileName))
            return;

        if(!data.read(itemRequest.size))
            return;

        if(!data.read(itemRequest.fileId))
            return;

        if(!data.read(itemRequest.md5))
            return;

        if(!data.read(itemRequest.accountId))
            return;

        if(!data.read(itemRequest.otherId))
            return;

        MessDiapatch::instance()->onRecvFileRequest(itemRequest);
    }
}

void DataProcess::proFileData(RBuffer &data)
{
    FileDataRequest request;

    int control = 0;

    if(!data.read(control))
        return;
    request.control = static_cast<FileTransferControl>(control);

    if(!data.read(request.fileId))
        return;

    if(!data.read(request.index))
        return;

    const char * recvData = NULL;
    size_t dataLen = 0;
    if(!data.read(&recvData,BUFFER_DEFAULT_SIZE,dataLen))
        return;
    request.array.append(recvData,dataLen);

    FileDesc * fileDesc = RSingleton<FileManager>::instance()->getFile(request.fileId);
    if(fileDesc)
    {
        if(fileDesc->isNull() && !fileDesc->create())
        {
            return;
        }

//        if(fileDesc->state() == FILE_TRANING || fileDesc->state() == FILE_PAUSE)
        if(fileDesc->seek(request.index * FILE_MAX_PACK_SIZE) && fileDesc->write(request.array) > 0)
        {
            qDebug()<<"++:"<<fileDesc->getWriteSize()<<":index:"<<request.index<<"_"<<fileDesc->fileSize();
            if(fileDesc->flush() && fileDesc->isRecvOver())
            {
                fileDesc->close();

                MessDiapatch::instance()->onRecvFileData(fileDesc->fileId,fileDesc->fileName);
            }
        }
    }
}
