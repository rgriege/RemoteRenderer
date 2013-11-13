#include <atomic>

extern "C" {
#include <libavcodec\avcodec.h>
}

struct EncodedFramePacket {
	EncodedFramePacket(AVPacket& pkt)
		: data(pkt.data), size(pkt.size), sent(false) {}
	uint8_t* data;
	int size;
	bool sent;
};