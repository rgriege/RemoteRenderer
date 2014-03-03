#include "GameApp.h"

GameApp::GameApp(std::string title)
    : mTitle(title)
{
}

void GameApp::_onMessage(client* client, connection_hdl hdl, message_ptr msg)
{
    int port = std::atoi(msg->get_raw_payload().c_str());
}

std::string GameApp::_getInitialRequest()
{
    return mTitle;
}

/*#include <GL/freeglut.h>
#include <Decoder.h>
#include <rgbData.h>

int width = 300;
int height = 300;
int buffer_size;
uint8_t* buffer;
Decoder* decoder;

void display()
{
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glutSwapBuffers();
}

void doStuff(int argc, char** argv) {
    decoder = new Decoder();
    decoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, width, height);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutCreateWindow("OpenGL Remote Rendering Client");
    glutDisplayFunc(display);

    printf("%d\n", glutGetWindow());

    width = glutGet(GLUT_WINDOW_WIDTH);
    height = glutGet(GLUT_WINDOW_HEIGHT);

    buffer_size = width*height*3;
    buffer = (uint8_t*) malloc(buffer_size);

    for (;;)
        glutMainLoopEvent();

    printf("Video playback complete\n");

    glutMainLoop();

    delete decoder;
    free(buffer);
}

void _onMessage(connection_hdl hdl, client::message_ptr msg)
{
    const void* data = msg->get_payload().c_str();
    decoder->decode_frame(static_cast<const uint8_t*>(data), msg->get_payload().length(), buffer);
    flip_image_vertically(buffer, width, height);
    glutPostRedisplay();
}*/
