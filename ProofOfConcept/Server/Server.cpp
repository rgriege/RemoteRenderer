#include "Server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include "NetworkManager.h"

Server* Server::instance = NULL;
SDL_Thread* Server::thread;
TCPsocket Server::serversocket;
host Server::hosts[MAX_HOSTS];
SDLNet_SocketSet Server::socketset;
std::string Server::hostname;
bool Server::quit = false;

int createServer(void *ptr)
{
	Server::GetInstance()->Start();
	return 0;
}

Server::Server()
{
	/* Make sure SDLNet is initialized */
	NeTwerk::NetworkManager::GetInstance();

	IPaddress ip;

	/* Initialize the channels */
	for (int i = 0; i < MAX_HOSTS; i++) {
		hosts[i].sock = NULL;
	}

	/* Allocate the socket set */
	socketset = SDLNet_AllocSocketSet(MAX_HOSTS+1);
	if (socketset == NULL) {
		fprintf(stderr, "Server: Couldn't create socket set %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	/* Resolving the host using NULL make network interface to listen */
	if (SDLNet_ResolveHost(&ip, NULL, GAME_PORT) < 0 || ip.host == INADDR_NONE) {
		fprintf(stderr, "Server: SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}

	/* Open a connection with the IP provided (listen on the host's port) */
	if (!(serversocket = SDLNet_TCP_Open(&ip))) {
		fprintf(stderr, "Server: SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
	SDLNet_TCP_AddSocket(socketset, serversocket);

	//const char* compname = NetworkManager::GetHostname().c_str();
	//std::cout << "You are hosting a chat on '" << compname << "'" << std::endl << flush;
	hostname = SDLNet_ResolveIP(&ip);
	std::cout << "hosting on " << hostname << " from " << ip.host << ":" << ip.port << std::endl;
}

Server* Server::GetInstance()
{
	if (!instance) {
		instance = new Server();
		thread = SDL_CreateThread(createServer, "ServerThread", NULL);
	}
	return instance;
}

void Server::Start()
{
	/* Wait for a connection, send data and term */
	quit = 0;
	while (!quit) {
		/* Wait for events */
		SDLNet_CheckSockets(socketset, TIMEOUT);
		/* Check for new connections */
		if (SDLNet_SocketReady(serversocket)) {
			AddClient();
		}
		/* Check for events on existing clients */
		for (int i = 0; i < MAX_HOSTS; i++) {
			if (SDLNet_SocketReady(hosts[i].sock)) {
				HandleClient(i);
			}
		}
	}

	std::cout << "Closing server..." << std::endl;

	return;
}

void Server::HandleClient(int idx)
{
	char data[512];
	/* Has the connection been closed? */
	if (SDLNet_TCP_Recv(hosts[idx].sock, data, 512) <= 0) {
		RemoveClient(idx);
		/*unsigned count = 0;
		for (int i = 0; i < MAX_HOSTS; i++)
		if (hosts[i].sock) count++;
		if (count == 0)
		quit = 1;*/
	} else {
		std::cout << "message from " << hosts[idx].name << std::endl;
		if (data[TYPE] == NAME) {
			hosts[idx].name = std::string(&data[1]);
			std::string msg = hosts[idx].name + " has joined the chat";
			for (int i = 0; i < MAX_HOSTS; i++)
				if (hosts[i].sock && i != idx)
					NeTwerk::NetworkManager::GetInstance()->SendMessage(msg, hosts[i].sock);
		} else {
			if (strcmp(&data[1], "exit") == 0 || strcmp(&data[1], "quit") == 0) {
				RemoveClient(idx);
			} else {
				std::string msg = hosts[idx].name + ": " + std::string(&data[1]);
				for (int i = 0; i < MAX_HOSTS; i++)
					if (hosts[i].sock)
						NeTwerk::NetworkManager::GetInstance()->SendMessage(msg, hosts[i].sock);
			}
		}
	}
}

void Server::AddClient()
{
	TCPsocket newsock;
	int idx;
	unsigned char data;
	newsock = SDLNet_TCP_Accept(serversocket);
	if (newsock == NULL) {
		printf("Invalid new client socket\n");
		return;
	}
	/* Look for unconnected slot */
	for (idx = 0; idx < MAX_HOSTS; idx++) {
		if (!hosts[idx].sock)
			break;
	}

	if (idx == MAX_HOSTS) {
		/* No room */
		memcpy(&data, "Goodbye", 8);
		SDLNet_TCP_Send(newsock, &data, 8);
		SDLNet_TCP_Close(newsock);
	} else {
		/* Add socket */
		hosts[idx].sock = newsock;
		hosts[idx].addr = *SDLNet_TCP_GetPeerAddress(newsock);
		SDLNet_TCP_AddSocket(socketset, hosts[idx].sock);
		std::cout << "adding client " << idx << std::endl;
	}
}

void Server::RemoveClient(int idx)
{
	std::string msg = hosts[idx].name + " has left the chat";
	std::cout << msg << std::endl;

	hosts[idx].name = "";
	hosts[idx].sock = NULL;
	SDLNet_TCP_DelSocket(socketset, hosts[idx].sock);
	SDLNet_TCP_Close(hosts[idx].sock);

	/* Notify all clients */
	for (int i = 0; i < MAX_HOSTS; i++)
		if (hosts[i].sock) 
			NeTwerk::NetworkManager::GetInstance()->SendMessage(msg, hosts[i].sock);
}

std::string Server::getHostname()
{
	return hostname;
}

void Server::Stop()
{
	quit = true;
	SDL_WaitThread(thread, NULL);
}