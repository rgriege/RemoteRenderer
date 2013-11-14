#include <GL\freeglut.h>

#include "GLWindowStream.h"

int GLWindowStream::read(int size, void* data)
{
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	if (size == width * height)
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (size <= width)
		glReadPixels(0, 0, size, 1, GL_RGB, GL_UNSIGNED_BYTE, data);
	else
		glReadPixels(0, 0, width, size / width, GL_RGB, GL_UNSIGNED_BYTE, data);
	return width * height;
}

void GLWindowStream::write(void* data, int item_size, int item_count)
{
	glDrawPixels(item_size, item_count, GL_RGB, GL_UNSIGNED_BYTE, data);
}