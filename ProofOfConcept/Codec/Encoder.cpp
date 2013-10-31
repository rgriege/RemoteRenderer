#include "Encoder.h"
#include <cassert>

bool Encoder::libInit = false;

Encoder::Encoder() {
    /* register all the codecs */
	if (!libInit) {
		avcodec_register_all();
		libInit = true;
	}

	context = NULL;
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

    context = avcodec_alloc_context3(codec);
    if (!context) {
        fprintf(stderr, "Could not allocate video codec context\n");
        return false;
    }

	/* put sample parameters */
    context->bit_rate = 400000;
    /* resolution must be a multiple of two */
    context->width = width;
    context->height = height;
    /* frames per second */
	AVRational avr = {1, fps};
    context->time_base = avr;
    context->gop_size = 10; /* emit one intra frame every ten frames */
    context->max_b_frames=1;
    context->pix_fmt = AV_PIX_FMT_YUV420P;

	if(codec->id == AV_CODEC_ID_H264)
        av_opt_set(context->priv_data, "preset", "slow", 0);
	
	ctx = sws_getContext(context->width, context->height,
						 AV_PIX_FMT_RGB24, context->width, context->height,
						 context->pix_fmt, 0, 0, 0, 0);

	return true;
}

bool Encoder::openCodec() {
	assert(context != NULL);
	assert(codec != NULL);

    if (avcodec_open2(context, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        return false;
    }
	return true;
}

bool Encoder::bootstrap(AVCodecID codecId, int width, int height, int fps) {
	return setCodec(codecId) && createContext(width, height, fps) && openCodec();
}

AVFrame* Encoder::createFrame() {
	assert(context != NULL);

	AVFrame* frame = avcodec_alloc_frame();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return NULL;
    }
    frame->format = context->pix_fmt;
    frame->width  = context->width;
    frame->height = context->height;
	
    /* the image can be allocated by any means and av_image_alloc() is
     * just the most convenient way if av_malloc() is to be used */
    int ret = av_image_alloc(frame->data, frame->linesize, context->width, context->height,
                         context->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        return NULL;
    }

	return frame;
}

bool Encoder::writeFrame(AVFrame* frame, FILE* file) {
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;    // packet data will be allocated by the encoder
    pkt.size = 0;
	int got_output;

	frame->pts = ++frameNum;

	/* encode the image */
	int ret = avcodec_encode_video2(context, &pkt, frame, &got_output);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		return false;
	}

	if (got_output) {
		printf("Write frame %3d (size=%5d)\n", frameNum, pkt.size);
		fwrite(pkt.data, 1, pkt.size, file);
		av_free_packet(&pkt);
	}
	return true;
}

void Encoder::destroyFrame(AVFrame* frame) {
    av_freep(&frame->data[0]);
    avcodec_free_frame(&frame);
}

void Encoder::convertRgbDataToFrame(const uint8_t* rgb24Data, AVFrame* frame) {
	const uint8_t * inData[1] = { rgb24Data }; // RGB24 have one plane
	int inLinesize[1] = { 3*context->width }; // RGB stride
	sws_scale(ctx, inData, inLinesize, 0, context->height, frame->data, frame->linesize);
}

void Encoder::writeEndFile(FILE* f) {
    fwrite(endcode, 1, sizeof(endcode), f);
}

Encoder::~Encoder() {
	if (context) {
		avcodec_close(context);
		av_free(context);
	}
}