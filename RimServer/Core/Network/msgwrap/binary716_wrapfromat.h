/*!
 *  @brief     716数据体封装格式
 *  @details   用于封装716原始数据信息
 *  @author    wey
 *  @version   1.0
 *  @date      2018.06.28
 *  @warning
 *  @copyright NanJing RenGu.
 */
#ifndef BINARY716_WRAPFROMAT_H
#define BINARY716_WRAPFROMAT_H

#include "wrapformat.h"

class Binary716_WrapFromat : public WrapFormat
{
public:
    explicit Binary716_WrapFromat();

    QByteArray handleMsg(MsgPacket * packet,int result = 0);

private:
    QByteArray handleText(TextRequest *);

};

#endif // BINARY716_WRAPFROMAT_H
