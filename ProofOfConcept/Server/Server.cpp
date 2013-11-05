#include "Server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include "NetworkManager.h"

Server* Server::instance = NULL;
//SDL_Thread* Server::thread;
uint32_t Server::serversocket;
client Server::clients[MAX_CLIENTS];
//SDLNet_SocketSet Server::socketset;
std::string Server::hostname;
bool Server::quit = false;
NeTwerk::NetworkManager* Server::netMgr = NULL;

/*int createServer(void *ptr)
{
	Server::GetInstance()->Start();
	return 0;
}*/

Server::Server()
{
	/* Make sure SDLNet is initialized, and create a socket to listen for connections */
	netMgr = NeTwerk::NetworkManager::getInstance();
	serversocket = netMgr->listen();

	//const char* compname = NetworkManager::GetHostname().c_str();
	//std::cout << "You are hosting a chat on '" << compname << "'" << std::endl << flush;
	hostname = netMgr->getLocalHostname();
	std::string ip = netMgr->getLocalIP();
	std::cout << "hosting on " << hostname << " from " << ip << std::endl;
}

Server* Server::getInstance()
{
	if (!instance) {
		instance = new Server();
		// thread = SDL_CreateThread(createServer, NULL);
	}
	return instance;
}

void Server::waitForConnection()
{
	while (!netMgr->check(serversocket, TIMEOUT)) {}
	addClient();
}

void Server::broadcast(void* data, int size) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].socketId)
			netMgr->send(data, size, clients[i].socketId);
	}
}

void Server::handleClient(int idx)
{
	char data[512];
	/* Has the connection been closed? */
	if (netMgr->receive(clients[idx].socketId, data, 512) <= 0) {
		removeClient(idx);
		unsigned count = 0;
		for (int i = 0; i < MAX_CLIENTS; i++)
			if (clients[i].socketId)
				count++;
		if (count == 0)
			quit = 1;
	} /*else {
		std::cout << "message from " << clients[idx].name << std::endl;
		if (data[TYPE] == NAME) {
			clients[idx].name = std::string(&data[1]);
			std::string msg = clients[idx].name + " has joined the chat";
			for (int i = 0; i < MAX_CLIENTS; i++)
				if (clients[i].socketId && i != idx)
					netMgr->send(msg, clients[i].socketId);
		} else {
			if (strcmp(&data[1], "exit") == 0 || strcmp(&data[1], "quit") == 0) {
				RemoveClient(idx);
			} else {
				std::string msg = clients[idx].name + ": " + std::string(&data[1]);
				for (int i = 0; i < MAX_CLIENTS; i++)
					if (clients[i].socketId)
						netMgr->send(msg, clients[i].socketId);
			}
		}
	}*/
}

void Server::addClient()
{
	/* Accept the connection */
	uint32_t clientSocketId = netMgr->accept(serversocket);

	int idx;
	/* Look for unconnected slot */
	for (idx = 0; idx < MAX_CLIENTS; idx++) {
		if (!clients[idx].socketId)
			break;
	}

	if (idx == MAX_CLIENTS) {
		/* No room */
		netMgr->send("Goodbye", 8, clientSocketId);
		netMgr->close(clientSocketId);
	} else {
		/* Add socket */
		clients[idx].socketId = clientSocketId;
		//clients[idx].ip = NeTwerk::NetworkManager::getInstance()->getIP(clientSocketId);
		//SDLNet_TCP_AddSocket(socketset, hosts[idx].sock);
		std::cout << "adding client " << idx << std::endl;
	}
}

void Server::removeClient(int idx)
{
	std::cout << clients[idx].name << " has left" << std::endl;
	netMgr->close(clients[idx].socketId);

	clients[idx].name = "";
	clients[idx].socketId = BAD_SOCKET_ID;
}

std::string Server::getHostname()
{
	return hostname;
}