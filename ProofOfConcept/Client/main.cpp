#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <iostream>

#include <GL/freeglut.h>

#include <NetworkManager.h>

#undef main
int main( int argc, char **argv )
{
	NeTwerk::NetworkManager* netMgr = NeTwerk::NetworkManager::getInstance();

	std::cout << "Enter server hostname: ";
	std::string server;
	std::cin >> server;
	std::cout << "Attempting to connect to '" << server << "'..." << std::endl;
	uint32_t serverSocketId = netMgr->connect(server.c_str());
	if (serverSocketId == BAD_SOCKET_ID)
		exit(EXIT_FAILURE);
	else
		std::cout << "connected to server!" << std::endl;

    unsigned int i;

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DEPTH|GLUT_DOUBLE );
    glutCreateWindow( "OpenGL Remote Rendering Client" );

    printf("%d\n",glutGetWindow());

    for (;;)
    {
		netMgr->prime();
		if (netMgr->receive(serverSocketId, data, 512)) {
			glutPostRedisplay();
		}
        glutMainLoopEvent();
    }

    glutMainLoop( );

    return EXIT_SUCCESS;
}