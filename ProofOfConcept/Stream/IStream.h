#ifndef __STREAM_H__
#define __STREAM_H__

#include "ExportDefine.h"

class STREAM_API IStream
{
public:
	virtual int read(int size, void* data) = 0;
	virtual void write(void* data, int item_size, int item_count) = 0;
};

#endif