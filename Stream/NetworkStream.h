#ifndef __NETWORK_STREAM_H__
#define __NETWORK_STREAM_H__

#include <NetworkManager.h>

#include "IStream.h"

class STREAM_API NetworkStream : public IStream
{
public:
    NetworkStream(uint32_t _socket_id);
    int read(int size, void* data);
    void write(void* data, int item_size, int item_count);
private:
    uint32_t socket_id;
    NeTwerk::NetworkManager* netMgr;
};

#endif