#ifndef __GL_WINDOW_STREAM_H__
#define __GL_WINDOW_STREAM_H__

#include <GL\freeglut.h>

#include "IStream.h"

class STREAM_API GLWindowStream : public IStream
{
public:
    int read(int size, void* data);
    void write(void* data, int item_size = glutGet(GLUT_WINDOW_WIDTH), int item_count = glutGet(GLUT_WINDOW_HEIGHT));
};

#endif