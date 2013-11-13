#include <Encoder.h>
#include <atomic>

struct EncodedFramePacket {
	EncodedFramePacket(AVPacket& pkt)
		: data(pkt.data), size(pkt.size), sent(false) {}
	uint8_t* data;
	int size;
	bool sent;
};

//std::atomic<EncodedFramePacket> currentFramePacket;
std::atomic<AVPacket> currentFramePacket;
std::atomic_bool newFrame;