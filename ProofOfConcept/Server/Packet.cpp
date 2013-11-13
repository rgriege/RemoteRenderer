#include "Packet.h"

//std::atomic<EncodedFramePacket> currentFramePacket;
std::atomic<AVPacket> currentFramePacket;
std::atomic_bool newFrame;