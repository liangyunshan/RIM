#ifndef BINARY_DATAPROCESS_H
#define BINARY_DATAPROCESS_H

#include "Util/rbuffer.h"

class Binary_DataProcess
{
public:
    Binary_DataProcess();

    void proFileControl(RBuffer &data);
    void proFileRequest(RBuffer & data);
    void proFileData(RBuffer & data);
};

#endif // BINARY_DATAPROCESS_H
