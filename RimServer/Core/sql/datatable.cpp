#include "datatable.h"

namespace DataTable {

RUser::RUser():
    table("ruser"),id("ID"),account("ACCOUNT"),
    password("PASSWORD"),nickName("NICKNAME"),signName("SIGNNAME"),
    gender("GENDER"),birthDay("BIRTHDAY"),phone("PHONE"),address("ADDRESS"),
    email("EMAIL"),remark("REMARK"),systemIon("SYSTEMICON"),iconId("ICONID")
{

}

RGroup::RGroup():table("rgroup"),
    id("ID"),name("NAME"),userCount("USER_COUNT"),
    userId("UID"),defaultGroup("DEFAUL"),index("SINDEX")
{

}

RChatRoom::RChatRoom():
    table("rchatroom"),id("ID"),chatId("CHATID"),name("NAME"),desc("DESCRIPT"),label("LABEL"),visible("VISIBLE"),
    validate("VALIDATE"),question("QUESTION"),answer("ANSWER"),userId("UID"),systemIon("SYSTEMICON"),iconId("ICONID")
{

}

RChatGroup::RChatGroup():table("rchatgroup"),id("ID"),name("NAME"),groupCount("GCOUNT"),userId("UID"),defaultGroup("DEFAULTGROUP")
{

}


RChatGroupDesc::RChatGroupDesc():table("rchatgroupdesc"),id("ID"),account("ACCOUNT"),chatgroupids("CHATGROUPIDS"),
    chatgroupsize("CHATSIZE")
{

}

RGroup_User::RGroup_User():table("rgroup_user"),
    id("ID"),groupId("GID"),userId("UID"),remarks("REMARKS"),visible("VISIBLE")
{

}

RChatroom_User::RChatroom_User():table("rchatroom_user"),
    id("ID"),chatroomId("CID"),userId("UID"),manager("MANAGER"),remarks("REMARKS")
{

}

RimConfig::RimConfig():table("RimConfig"),name("NAME"),value("VALUE"),accountId("ACCOUNT_ID"),
    groupAccoungId("CHATROOM_ID")
{

}

RequestCache::RequestCache():table("requestcache"),id("ID"),account("ACCOUNT"),
    operateId("OPERATEID"),type("TYPE"),time("TIME")
{

}

RUserChatCache::RUserChatCache():table("ruserchatcache"),id("ID"),account("ACCOUNT"),
otherSideId("OTHERSIDEID"),data("DATA"),time("TTSTAMP"),msgType("MSGTYPE"),textId("TEXTID"),textType("TEXTTYPE"),encryption("ENCRYPTION"),
  compress("COMPRESS")
{

}

RFile::RFile():table("rfile"),id("ID"),md5("MD5"),fileName("FILENAME"),src("SRC"),dst("DST"),dtime("DTIME"),fileSize("FILESIZE"),
    quoteId("QUOTEID"),quoteNum("QUOTENUM"),filePath("FILEPATH")
{

}

RGroupDesc::RGroupDesc():table("rgroupdesc"),id("ID"),account("USERID"),groupids("GROUPIDS"),groupsize("GROUPSIZE"){

}

RChatGroupRoom::RChatGroupRoom():table("rchatgrouproom"),id("ID"),chatroomId("CHATROOMID"),chatgroupId("CHATGROUPID"),
    remarks("REMARKS"),messNotifyLevel("NOTIFYLEVEL")
{

}

#ifdef __LOCAL_CONTACT__
RChat716Cache::RChat716Cache():table("rchat716cache"),sourceAddr("SOURCEADDR"),destAddr("DESTADDR"),
    packType("PACKTYPE"),reserve("RESERVE"),fileType("FILETYPE"),fileName("FILENAME"),data("DATA"),
    serialNo("SERIALNO"),orderNo("ORDERNO"),date("CDATE"),time("CTIME")
{

}
#endif


}


