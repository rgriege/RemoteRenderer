#ifndef __DECODER_H__
#define __DECODER_H__

#include <math.h>

extern "C" {
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>
}

#define INBUF_SIZE 4096

#include "CodecPrereqs.h"

class CODEC_API Decoder
{
public:
    Decoder();
    ~Decoder();

    bool setCodec(AVCodecID codecId);
    bool createContext(int width, int height);
    bool openCodec();
    bool createFrame();
    bool bootstrap(AVCodecID codecId, int width, int height);

    bool decode_frame(const uint8_t* encodedData, uint32_t encodedDataLen, uint8_t* const rgbData);

private:
    AVCodec* codec;
    AVCodecContext* codec_context;
    AVFrame* frame;
    uint32_t frame_num;
    AVPacket current_packet;
    uint8_t* input_buffer;

    SwsContext* conversion_context;
};

#endif
