#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <GL/freeglut.h>
#include <NetworkManager.h>
#include <Decoder.h>
#include <rgbData.h>
#include <NetworkStream.h>

int width = 300;
int height = 300;
int buffer_size;
uint8_t* buffer;
Decoder* decoder;

void cb_display()
{
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glutSwapBuffers();
}

#undef main
int main( int argc, char **argv )
{
    NeTwerk::NetworkManager* netMgr = NeTwerk::NetworkManager::getInstance();

    std::cout << "Enter server hostname: ";
    std::string server = netMgr->getLocalHostname();
    //std::cin >> server;
    std::cout << "Attempting to connect to '" << server << "'..." << std::endl;
    uint32_t serverSocketId = netMgr->connect(server.c_str());
    if (serverSocketId == BAD_SOCKET_ID)
        exit(EXIT_FAILURE);
    else
        std::cout << "connected to server!" << std::endl;

    decoder = new Decoder();
    decoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, width, height);

    NetworkStream input_stream(serverSocketId);

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    glutInitWindowSize(width, height);
    glutCreateWindow( "OpenGL Remote Rendering Client");
    glutDisplayFunc( cb_display );

    printf("%d\n",glutGetWindow());

    width = glutGet(GLUT_WINDOW_WIDTH);
    height = glutGet(GLUT_WINDOW_HEIGHT);

    buffer_size = width*height*3;
    buffer = (uint8_t*) malloc(buffer_size);

    int bytes_read = 0;
    for (;;)
    {
        netMgr->prime();
        decoder->decode_frame(input_stream, buffer);
        flip_image_vertically(buffer, width, height);
        glutPostRedisplay();

        glutMainLoopEvent();
    }

    printf("Video playback complete\n");

    glutMainLoop( );

    delete decoder;
    free(buffer);

    return EXIT_SUCCESS;
}