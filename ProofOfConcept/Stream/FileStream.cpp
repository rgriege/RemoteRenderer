#include "FileStream.h"

FileStream::FileStream(FILE* file)
	: f(file)
{
}

int FileStream::read(int size, void* data)
{
	return fread(data, 1, size, f);
}

void FileStream::write(void* data, int item_size, int item_count)
{
	fwrite(data, item_size, item_count, f);
}