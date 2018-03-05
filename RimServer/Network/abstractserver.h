/*!
 *  @brief     抽象服务器类
 *  @details   为应用层提供统一的访问接口，不同的平台以及传输方式继承此类。
 *  @file      abstractserver.h
 *  @author    wey
 *  @version   1.0
 *  @date      2018.03.02
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H

namespace ServerNetwork {

class AbstractServer
{
public:
    explicit AbstractServer();

    virtual bool startMe(const char * ip,unsigned short port) = 0;

};

} //namespace ServerNetwork

#endif // ABSTRACTSERVER_H
