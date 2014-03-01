#include <cassert>
#include <algorithm>

#include "Decoder.h"
#include "Init.h"

Decoder::Decoder() {
    init_av_codec();

    codec = NULL;
    codec_context = NULL;
    conversion_context = NULL;
    frame_num = 0;
    av_init_packet(&current_packet);
    current_packet.size = 0;
    input_buffer = (uint8_t*) malloc(INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE);
}

bool Decoder::setCodec(AVCodecID codecId) {
    codec = avcodec_find_decoder(codecId);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        return false;
    }
    return true;
}

bool Decoder::createContext(int width, int height) {
    assert(codec != NULL);

    codec_context = avcodec_alloc_context3(codec);
    if (!codec_context) {
        fprintf(stderr, "Could not allocate video codec codec_context\n");
        return false;
    }
    
    codec_context->width = width;
    codec_context->height = height;

    if (codec->capabilities & CODEC_CAP_TRUNCATED)
        codec_context->flags = CODEC_FLAG_TRUNCATED; /* we do not send complete frames */

    /*conversion_context = sws_getCachedContext(NULL, codec_context->width, codec_context->height,
    codec_context->pix_fmt, codec_context->width, codec_context->height, AV_PIX_FMT_RGB24, 0, 0, 0, 0);*/
    /*conversion_context = sws_getContext(codec_context->width, codec_context->height,
    codec_context->pix_fmt, codec_context->width, codec_context->height,
    AV_PIX_FMT_RGB24, 0, 0, 0, 0);*/

    return true;
}

bool Decoder::openCodec() {
    assert(codec_context != NULL);
    assert(codec != NULL);

    if (avcodec_open2(codec_context, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return false;
    }
    return true;
}

bool Decoder::createFrame() {
    assert(codec_context != NULL);

    frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return false;
    }
    frame->width  = codec_context->width;
    frame->height = codec_context->height;
    return true;
}

bool Decoder::bootstrap(AVCodecID codecId, int width, int height) {
    return setCodec(codecId) && createContext(width, height) && openCodec() && createFrame();
}

bool Decoder::decode_frame(const uint8_t* encodedData, uint32_t encodedDataLen, uint8_t* const rgbData)
{
    uint32_t encodedDataPos = 0;
    int got_frame = 0;
    while (!got_frame) {
        /* If needed, get new data */
        if (current_packet.size == 0) {
            /* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
            memset(input_buffer + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

            current_packet.size = std::min(static_cast<uint32_t>(INBUF_SIZE), encodedDataLen - encodedDataPos);
            memcpy(input_buffer, encodedData + encodedDataPos, current_packet.size);
            encodedDataPos += current_packet.size;
            if (current_packet.size == 0)
                return false;

            current_packet.data = input_buffer;
        }

        int len = avcodec_decode_video2(codec_context, frame, &got_frame, &current_packet);
        if (len < 0) {
            fprintf(stderr, "Error while decoding frame %d\n", frame_num);
            return false;
        }
        if (current_packet.data) {
            current_packet.size -= len;
            current_packet.data += len;
        }
    }
    //printf("Showing frame %3d\n", frame_num);
    fflush(stdout);
    frame_num++;
    uint8_t * const outData[1] = { rgbData }; // RGB24 have one plane
    int outLinesize[1] = { 3*codec_context->width }; // RGB stride
    if (!conversion_context) {
        conversion_context = sws_getCachedContext(NULL, codec_context->width, codec_context->height,
            codec_context->pix_fmt, codec_context->width, codec_context->height, AV_PIX_FMT_RGB24, 0, 0, 0, 0);
    }
    sws_scale(conversion_context, frame->data, frame->linesize, 0, frame->height, outData, outLinesize);
    return true;
}

Decoder::~Decoder() {
    if (frame)
        avcodec_free_frame(&frame);
    if (codec_context) {
        avcodec_close(codec_context);
        av_free(codec_context);
    }
}