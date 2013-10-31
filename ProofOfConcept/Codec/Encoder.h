#ifndef __ENCODER_H__
#define __ENCODER_H__

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

#define INBUF_SIZE 4096

#ifdef CODEC_EXPORT
    #define CODEC_API __declspec(dllexport)
#else
    #define CODEC_API __declspec(dllimport)
#endif

class CODEC_API Encoder
{
public:
	Encoder();
	~Encoder();

	bool setCodec(AVCodecID codecId);
	bool createContext(int width, int height, int fps);
	bool openCodec();
	bool bootstrap(AVCodecID codecId, int width, int height, int fps);
	
	AVFrame* createFrame();
	bool writeFrame(AVFrame* frame, FILE* file);
	void destroyFrame(AVFrame* frame);

	void writeEndFile(FILE* file);

	void convertRgbDataToFrame(const uint8_t* rgbData, AVFrame* frame);

private:
	AVCodec* codec;
    AVCodecContext* context;
    uint8_t endcode[4];
	uint32_t frameNum;

	SwsContext* ctx;

	static bool libInit;
};

#endif