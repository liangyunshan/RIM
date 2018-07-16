#include "binary716_msgparse.h"

#include "util/rsingleton.h"
#include "../msgprocess/data716process.h"

#ifdef __LOCAL_CONTACT__

Binary716_MsgParse::Binary716_MsgParse():
    DataParse()
{

}

/*!
 * @brief 处理去除协议头后的原始数据
 * @param[in] packData 原始数据+部分传输字段
 */
void Binary716_MsgParse::processData(const ProtocolPackage &packData)
{
    if(packData.cFileType == QDB2051::F_NO_SUFFIX){
        switch(packData.bPackType){
            case T_DATA_AFFIRM:
                {
                    RSingleton<Data716Process>::instance()->processTextAffirm(packData);
                }
                break;
            case T_DATA_NOAFFIRM:
                {
                    RSingleton<Data716Process>::instance()->processTextNoAffirm(packData);
                }
                break;
            case T_DATA_REG:
                break;

            default:break;
        }
    }
    else if(packData.cFileType == QDB2051::F_TEXT || packData.cFileType == QDB2051::F_BINARY)
    {
        RSingleton<Data716Process>::instance()->processFileData(packData);
    }
}

#endif
