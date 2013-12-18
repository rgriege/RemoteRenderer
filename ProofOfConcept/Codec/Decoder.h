#ifndef __DECODER_H__
#define __DECODER_H__

#include <math.h>

extern "C" {
#include <libavutil\opt.h>
#include <libavcodec\avcodec.h>
#include <libavutil\channel_layout.h>
#include <libavutil\common.h>
#include <libavutil\imgutils.h>
#include <libavutil\mathematics.h>
#include <libavutil\samplefmt.h>
#include <libswscale\swscale.h>
}

#include "IStream.h"

#define INBUF_SIZE 4096

#ifdef CODEC_EXPORT
#define CODEC_API __declspec(dllexport)
#else
#define CODEC_API __declspec(dllimport)
#endif

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

    bool decode_frame(IStream& stream, uint8_t* const rgbData);

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