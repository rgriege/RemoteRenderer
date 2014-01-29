#include "rgbData.h"

void flip_image_vertically(uint8_t* buffer, int width, int height)
{
    for (int i = 0; i < height/2; i++) {
        for (int j = 0; j < width; j++) {
            int top = (i*width + j) * 3;
            int bottom = ((height-i-1)*width + j) * 3;
            rgbData temp(&buffer[top]);
            ((rgbData*) &buffer[bottom])->copyTo(&buffer[top]);
            temp.copyTo(&buffer[bottom]);
        }
    }
}