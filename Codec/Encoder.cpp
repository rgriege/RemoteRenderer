#include <cassert>

#include "Encoder.h"
#include "Init.h"

Encoder::Encoder() {
    init_av_codec();

    codec_context = NULL;
    //endcode = { 0, 0, 1, 0xb7 };
    endcode[0] = 0;
    endcode[1] = 0;
    endcode[2] = 1;
    endcode[3] = 0xb7;
    frame_num = 0;
}

bool Encoder::setCodec(AVCodecID codecId) {
    codec = avcodec_find_encoder(codecId);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        return false;
    }
    return true;
}

bool Encoder::createContext(int width, int height, int fps) {
    assert(codec != NULL);

    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        fprintf(stderr, "Could not allocate video codec codec_context\n");
        return false;
    }

    /* put sample parameters */
    codec_context->bit_rate = 400000;
    /* resolution must be a multiple of two */
    codec_context->width = width;
    codec_context->height = height;
    /* frames per second */
    AVRational avr = {1, fps};
    codec_context->time_base = avr;
    codec_context->gop_size = 10; /* emit one intra frame every ten frames */
    codec_context->max_b_frames=1;
    codec_context->pix_fmt = AV_PIX_FMT_YUV420P;

    if(codec->id == AV_CODEC_ID_H264)
        av_opt_set(codec_context->priv_data, "preset", "slow", 0);

    conversion_context = sws_getContext(codec_context->width, codec_context->height,
        AV_PIX_FMT_RGB24, codec_context->width, codec_context->height,
        codec_context->pix_fmt, 0, 0, 0, 0);

    return true;
}

bool Encoder::openCodec() {
    assert(codec_context != NULL);
    assert(codec != NULL);

    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return false;
    }
    return true;
}

bool Encoder::createFrame() {
    assert(codec_context != NULL);

    frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return false;
    }
    frame->format = codec_context->pix_fmt;
    frame->width  = codec_context->width;
    frame->height = codec_context->height;

    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(frame->data, frame->linesize, codec_context->width, codec_context->height,
        codec_context->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        return false;
    }

    return true;
}

bool Encoder::bootstrap(AVCodecID codecId, int width, int height, int fps) {
    return setCodec(codecId) && createContext(width, height, fps) && openCodec() && createFrame();
}

void Encoder::write_rgb_data_to_frame(const uint8_t* rgbData)
{
    /* write the rgbData to the frame */
    const uint8_t * inData[1] = { rgbData }; // RGB24 have one plane
    int inLinesize[1] = { 3*codec_context->width }; // RGB stride
    sws_scale(conversion_context, inData, inLinesize, 0, codec_context->height, frame->data, frame->linesize);
}

void Encoder::encode_frame(AVPacket& pkt) {
    pkt.data = NULL; // packet data will be allocated by the encoder
    pkt.size = 0;
    int got_output;

    frame->pts = ++frame_num;

    /* encode the frame and write it to the packet */
    int ret = avcodec_encode_video2(codec_context, &pkt, frame, &got_output);
    if (ret < 0) {
        fprintf(stderr, "Error encoding frame\n");
        return;
    }

    /*if (got_output) {
        printf("Write frame %3d (size=%5d)\n", frame_num, pkt.size);
    }*/
}

void Encoder::writeEndFile(FILE* f) {
    fwrite(endcode, 1, sizeof(endcode), f);
}

Encoder::~Encoder() {
    if (frame) {
        av_freep(&frame->data[0]);
        avcodec_free_frame(&frame);
    }
    if (codec_context) {
        avcodec_close(codec_context);
        av_free(codec_context);
    }
}