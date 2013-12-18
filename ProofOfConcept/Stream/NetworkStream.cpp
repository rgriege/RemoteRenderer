#include "NetworkStream.h"

NetworkStream::NetworkStream(uint32_t _socket_id)
    : socket_id(_socket_id), netMgr(NeTwerk::NetworkManager::getInstance())
{
}

int NetworkStream::read(int size, void* data)
{
    return netMgr->receive(socket_id, data, size);
}

void NetworkStream::write(void* data, int item_size, int item_count)
{
    netMgr->send(data, item_size * item_count, socket_id);
}