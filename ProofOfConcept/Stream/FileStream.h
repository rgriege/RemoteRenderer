#ifndef __FILE_STREAM_H__
#define __FILE_STREAM_H__

#include <cstdio>

#include "IStream.h"

class STREAM_API FileStream : public IStream
{
public:
    FileStream(FILE* file);
    int read(int size, void* data);
    void write(void* data, int item_size, int item_count);
private:
    FILE* f;
};

#endif