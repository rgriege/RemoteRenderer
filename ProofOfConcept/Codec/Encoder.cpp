#include <cassert>

#include "Encoder.h"
#include "Init.h"

Encoder::Encoder() {
	init_av_codec();

	codecContext = NULL;
	//endcode = { 0, 0, 1, 0xb7 };
	endcode[0] = 0;
	endcode[1] = 0;
	endcode[2] = 1;
	endcode[3] = 0xb7;
	frameNum = 0;
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

    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        fprintf(stderr, "Could not allocate video codec codecContext\n");
        return false;
    }

	/* put sample parameters */
    codecContext->bit_rate = 400000;
    /* resolution must be a multiple of two */
    codecContext->width = width;
    codecContext->height = height;
    /* frames per second */
	AVRational avr = {1, fps};
    codecContext->time_base = avr;
    codecContext->gop_size = 10; /* emit one intra frame every ten frames */
    codecContext->max_b_frames=1;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

	if(codec->id == AV_CODEC_ID_H264)
        av_opt_set(codecContext->priv_data, "preset", "slow", 0);
	
	conversionContext = sws_getContext(codecContext->width, codecContext->height,
						 AV_PIX_FMT_RGB24, codecContext->width, codecContext->height,
						 codecContext->pix_fmt, 0, 0, 0, 0);

	return true;
}

bool Encoder::openCodec() {
	assert(codecContext != NULL);
	assert(codec != NULL);

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return false;
    }
	return true;
}

bool Encoder::createFrame() {
	assert(codecContext != NULL);

	frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return false;
    }
    frame->format = codecContext->pix_fmt;
    frame->width  = codecContext->width;
    frame->height = codecContext->height;
	
    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(frame->data, frame->linesize, codecContext->width, codecContext->height,
                         codecContext->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        return false;
    }

	return true;
}

bool Encoder::bootstrap(AVCodecID codecId, int width, int height, int fps) {
	return setCodec(codecId) && createContext(width, height, fps) && openCodec() && createFrame();
}

AVPacket* Encoder::encodeRgbData(const uint8_t* rgbData) {
	/* write the rgbData to the frame */
	const uint8_t * inData[1] = { rgbData }; // RGB24 have one plane
	int inLinesize[1] = { 3*codecContext->width }; // RGB stride
	sws_scale(conversionContext, inData, inLinesize, 0, codecContext->height, frame->data, frame->linesize);

	/* create the packet */
	AVPacket* pkt = new AVPacket();
	av_init_packet(pkt);
	pkt->data = NULL; // packet data will be allocated by the encoder
    pkt->size = 0;
	int got_output;

	frame->pts = ++frameNum;

	/* encode the frame and write it to the packet */
	int ret = avcodec_encode_video2(codecContext, pkt, frame, &got_output);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		return false;
	}

	if (got_output) {
		printf("Write frame %3d (size=%5d)\n", frameNum, pkt->size);
	}
	return pkt;
}

void Encoder::writeEndFile(FILE* f) {
    fwrite(endcode, 1, sizeof(endcode), f);
}

Encoder::~Encoder() {
	if (frame) {
		av_freep(&frame->data[0]);
		avcodec_free_frame(&frame);
	}
	if (codecContext) {
		avcodec_close(codecContext);
		av_free(codecContext);
	}
}