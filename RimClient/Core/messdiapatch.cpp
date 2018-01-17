#include "messdiapatch.h"

MessDiapatch * MessDiapatch::dispatch = NULL;

MessDiapatch::MessDiapatch(QObject *parent) : QObject(parent)
{

}

MessDiapatch *MessDiapatch::instance()
{
    if(dispatch == NULL)
    {
        dispatch = new MessDiapatch();
    }

    return dispatch;
}

void MessDiapatch::onRecvRegistResponse(ResponseRegister status, RegistResponse response)
{
    emit recvRegistResponse(status,response);
}
