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
    userId("UID"),defaultGroup("DEFAUL")
{

}

RChatRoom::RChatRoom():
    table("rchatroom"),id("ID"),name("NAME"),desc("DESC"),label("LABEL"),userId("UID")
{

}

RGroup_User::RGroup_User():table("rgroup_user"),
    id("ID"),groupId("GID"),userId("UID"),remarks("REMARKS"),visible("VISIBLE")
{

}

RChatroom_User::RChatroom_User():
    id("ID"),chatId("CID"),userId("UID")
{

}

RimConfig::RimConfig():table("RimConfig"),name("NAME"),value("VALUE"),accuoutId("ACCOUNT_ID")
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

}


