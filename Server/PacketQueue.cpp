#include "ConcurrentQueue.h"

extern "C" {
#include <libavcodec\avcodec.h>
}

ConcurrentQueue<AVPacket*> packet_queue;