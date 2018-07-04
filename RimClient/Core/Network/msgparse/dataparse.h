/*!
 *  @brief     数据解析
 *  @details   响应线程调用，验证数据合法性，解析数据类型； @n
 *             目前支持JSON、RBuffer方式数据解析，若以后想支持更多的封装类型，则可以继承此类，并实现对应的方法。
 *  @file      dataprocess.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.01.16
 *  @attention 服务器根据启动的服务类型，自动选择对应的解析方式， @n
 *             1.文本传输服务：采用字符型解析方式，通过使用JSON来加快解析的速度 @n
 *             2.文件传输服务：采用字节型解析方式，通过RBuffer获取原始的字节流，然后按位解析 @n
 *  @copyright NanJing RenGu.
 */
#ifndef DATAPARSE_H
#define DATAPARSE_H

#include "Network/network_global.h"

class DataParse
{
public:
    DataParse();

#ifdef __LOCAL_CONTACT__
    virtual void processData(const ProtocolPackage & recvData) = 0;
#else
    virtual void processData(const ProtocolPackage & recvData) = 0;
#endif
};

#endif // DATAPARSE_H
