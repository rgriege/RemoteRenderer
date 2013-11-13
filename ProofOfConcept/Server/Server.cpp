#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include "Server.h"
#include "Packet.h"

Server* Server::instance = NULL;
SDL_Thread* Server::thread;
bool Server::quit = false;

int createServer(void *ptr)
{
	Server::getInstance()->start();
	return 0;
}

Server::Server()
{
	/* Make sure SDLNet is initialized */
	netMgr = NeTwerk::NetworkManager::getInstance();

	listenSocketId = netMgr->listen();

	std::string hostname = netMgr->getLocalHostname();
	std::string ip = netMgr->getLocalIP();
	std::cout << "hosting on " << hostname << " from " << ip << std::endl;
}

Server* Server::getInstance()
{
	if (!instance) {
		instance = new Server();
		thread = SDL_CreateThread(createServer, NULL);
	}
	return instance;
}

void Server::start()
{
	/* Wait for a connection, send data and term */
	quit = 0;
	while (!quit) {
		/* Wait for events */
		netMgr->prime(0);
		/* Check for new connections */
		if (netMgr->check(listenSocketId)) {
			addClient();
		}
		/* Check for events on existing clients */
		for (socketIdVector::iterator it = clientSocketIds.begin(); it != clientSocketIds.end(); ++it) {
			if (netMgr->check(*it))
				handleClient(*it);
		}
		/* Broadcast the frame */
		if (newFrame) {
			AVPacket pkt = currentFramePacket;
			for (socketIdVector::iterator it = clientSocketIds.begin(); it != clientSocketIds.end(); ++it) {
				netMgr->send(pkt.data, pkt.size, *it);
			}
		}
	}

	std::cout << "Closing server..." << std::endl;

	return;
}

void Server::handleClient(uint32_t socketId)
{
	char data[512];
	/* Has the connection been closed? */
	if (netMgr->receive(socketId, data, 512) <= 0) {
		removeClient(socketId);
		/*unsigned count = 0;
		for (int i = 0; i < MAX_HOSTS; i++)
			if (hosts[i].sock) count++;
		if (count == 0)
		quit = 1;*/
	} else {
		if (data[TYPE] == NAME) {

		} else {
			if (strcmp(&data[1], "exit") == 0 || strcmp(&data[1], "quit") == 0) {
				removeClient(socketId);
			} else {

			}
		}
	}
}

void Server::addClient()
{
	uint32_t newsock = netMgr->accept(listenSocketId);
	if (newsock == NULL) {
		printf("Invalid new client socket\n");
		return;
	}
	clientSocketIds.push_back(newsock);
}

void Server::removeClient(uint32_t socketId)
{
	netMgr->close(socketId);
	clientSocketIds.erase(std::find(clientSocketIds.begin(), clientSocketIds.end(), socketId));
}

void Server::stop()
{
	quit = true;
	SDL_WaitThread(thread, NULL);
}