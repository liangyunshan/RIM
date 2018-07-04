#ifndef MSGWRAP_H
#define MSGWRAP_H

#ifdef __LOCAL_CONTACT__
#include "../../protocol/datastruct.h"
using namespace ParameterSettings;
#else
#include "../../protocol/protocoldata.h"
using namespace ProtocolType;
#endif

class MsgWrap
{
public:
    explicit MsgWrap();

    virtual void handleMsg(MsgPacket * packet
#ifdef __LOCAL_CONTACT__
                           ,CommucationMethod method = C_None,MessageFormat format = M_NONE
#endif
            ) = 0;
};

#endif // MSGWRAP_H
