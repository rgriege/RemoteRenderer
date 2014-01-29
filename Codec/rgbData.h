#include <inttypes.h>

#ifdef CODEC_EXPORT
#define CODEC_API __declspec(dllexport)
#else
#define CODEC_API __declspec(dllimport)
#endif

struct rgbData {
    rgbData(uint8_t* data) {
        r = data[0];
        g = data[1];
        b = data[2];
    }
    void copyTo(uint8_t* data) {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

void CODEC_API flip_image_vertically(uint8_t* buffer, int width, int height);