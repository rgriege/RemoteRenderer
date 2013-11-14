#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include <GL/freeglut.h>
//#include <NetworkManager.h>
#include <Decoder.h>
#include <rgbData.h>

int width;
int height;
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
	/*NeTwerk::NetworkManager* netMgr = NeTwerk::NetworkManager::getInstance();

	std::cout << "Enter server hostname: ";
	std::string server;
	std::cin >> server;
	std::cout << "Attempting to connect to '" << server << "'..." << std::endl;
	uint32_t serverSocketId = netMgr->connect(server.c_str());
	if (serverSocketId == BAD_SOCKET_ID)
		exit(EXIT_FAILURE);
	else
		std::cout << "connected to server!" << std::endl;*/

	decoder = new Decoder();
	decoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO);

	FILE* f = fopen("test.mpg", "rb");
	if (!f) {
        fprintf(stderr, "Could not open %s\n", "test.mpg");
        exit(1);
    }

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowSize(352, 288);
	//glutInitWindowSize(decoder->getFrameWidth(), decoder->getFrameHeight());
    glutCreateWindow( "OpenGL Remote Rendering Client");
    glutDisplayFunc( cb_display );

    printf("%d\n",glutGetWindow());
	
	width = glutGet(GLUT_WINDOW_WIDTH);
	height = glutGet(GLUT_WINDOW_HEIGHT);

	buffer = (uint8_t*) malloc(width*height*3);

    for (;;)
    {
		/*netMgr->prime();
		if (netMgr->receive(serverSocketId, buffer, 2000)) {
			AVPacket* pkt = encoder->encodeRgbData(buffer);
			av_free_packet(pkt);
			glutPostRedisplay();
		}*/
		if (decoder->writeFrame(f, buffer)) {
			flip_image_vertically(buffer, width, height);
			glutPostRedisplay();
		} else {
			break;
		}

        glutMainLoopEvent();
    }

	printf("Video playback complete\n");

    glutMainLoop( );

    return EXIT_SUCCESS;
}