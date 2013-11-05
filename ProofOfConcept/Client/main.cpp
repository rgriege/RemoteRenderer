#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iostream>

#include <GL/freeglut.h>

#include <NetworkManager.h>

#undef main
int main( int argc, char **argv )
{
	std::cout << "Enter server hostname: ";
	std::string server;
	std::cin >> server;
	std::cout << "Attempting to connect to '" << server << "'..." << std::endl;
	uint32_t serverId = NeTwerk::NetworkManager::getInstance()->connect(server.c_str(), DEFAULT_PORT);
	if (serverId != BAD_SOCKET_ID)
		std::cout << "connected to server!" << std::endl;
	else
		exit(EXIT_FAILURE);

    /*unsigned int i;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DEPTH|GLUT_DOUBLE );
    glutCreateWindow( "OpenGLUT fonts" );

    printf("%d\n",glutGetWindow());

    for (;;)
    {
        glutPostRedisplay();
        glutMainLoopEvent();
    }

    glutMainLoop( );*/

    return EXIT_SUCCESS;
}