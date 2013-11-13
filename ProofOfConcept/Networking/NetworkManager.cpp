#include "NetworkManager.h"

#include <sstream>
#include <iostream>

#define SDL_INADDR_ANY INADDR_ANY
#define SDL_INADDR_BROADCAST INADDR_BROADCAST
#define SDL_INADDR_NONE INADDR_NONE

#ifdef _WIN32
#undef INADDR_ANY
#undef INADDR_BROADCAST
#undef INADDR_NONE
#include <WinSock.h>
#endif

namespace NeTwerk {

NetworkManager* NetworkManager::instance = NULL;

NetworkManager::NetworkManager()
{
	/* Initialize SDLnet */
	if (SDLNet_Init() < 0) {
		std::cout << "Couldn't initialize net:" << SDLNet_GetError() << std::endl;
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	/* Allocate the socket set */
	socketSet = SDLNet_AllocSocketSet(2);
	if (socketSet == NULL) {
		std::cout << "Couldn't create socket set" << SDLNet_GetError();
		exit(EXIT_FAILURE);
	}
}

NetworkManager* NetworkManager::getInstance()
{
	if (!instance)
		instance = new NetworkManager();
	return instance;
}

std::string NetworkManager::getLocalHostname()
{
	char compname[32];
#ifdef _WIN32
	WSADATA wsa_data;
	/* Load Winsock 2.0 DLL */
	if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0) {
		std::cerr << "WSAStartup() failed" << WSAGetLastError() << std::endl;
		exit (EXIT_FAILURE);
	}
	if(gethostname(compname, 31))
		std::cout << "error: " << WSAGetLastError() << std::endl;
	WSACleanup(); /* Cleanup Winsock */
#elif __linux__
	gethostname(compname, 32);
#endif
	std::string result(compname);
	return result;
}

std::string NetworkManager::getLocalIP()
{
	const char* compname = getLocalHostname().c_str();
	
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, compname, 0) < 0) {
		std::cout << "Couldn't get IP: " << SDLNet_GetError() << std::endl;
		return NULL;
	}

	return printIP(ip.host);
}

std::string NetworkManager::getIP(uint32_t socketId)
{
	IPaddress* ip = SDLNet_TCP_GetPeerAddress(sockets.at(socketId));
	return ip == NULL ? "" : printIP(ip->host);
}

uint32_t NetworkManager::listen(uint16_t port) {
	/* Set up a socket on this machine */
	return connect((char*) NULL, port);
}

uint32_t NetworkManager::accept(uint32_t listenSocketId) {
	TCPsocket socket = SDLNet_TCP_Accept(sockets.at(listenSocketId));

	if (socket == NULL) {
		std::cout << "Invalid new client socket" << std::endl;
		return BAD_SOCKET_ID;
	}

	return addSocket(socket);
}

uint32_t NetworkManager::connect(const char* host, uint16_t port)
{
	IPaddress ip;
	/* Resolve the host we are connecting to */
	if (SDLNet_ResolveHost(&ip, host, port) < 0 || ip.host == SDL_INADDR_NONE) {
		std::cout << "SDLNet_ResolveHost (" << host << "): " << SDLNet_GetError()
			<< std::endl;
		return BAD_SOCKET_ID;
	}

	return connect(ip.host, ip.port);
}

uint32_t NetworkManager::connect(uint32_t host, uint16_t port) {
	IPaddress ip = { host, port };
	TCPsocket socket;
	/* Open a connection with the IP provided */
	if (!(socket = SDLNet_TCP_Open(&ip))) {
		std::cout << "SDLNet_TCP_Open (" << printIP(host) << ":" << port << "): " << SDLNet_GetError() << std::endl;
		return BAD_SOCKET_ID;
	}

	return addSocket(socket);
}

bool NetworkManager::prime(uint32_t timeout)
{
	int count = SDLNet_CheckSockets(socketSet, timeout);
	if (count < 0) {
		std::cout << "SDLNet_TCP_CheckSockets: " << SDLNet_GetError();
		exit(EXIT_FAILURE);
	}
	return count > 0;
}

bool NetworkManager::check(uint32_t socketId)
{
	return SDLNet_SocketReady(sockets.at(socketId));
}

void NetworkManager::send(void* data, int size, uint32_t socketId)
{
	SDLNet_TCP_Send(sockets.at(socketId), data, size);
}

void NetworkManager::send(std::string message, uint32_t socketId)
{
	send((void*) message.c_str(), message.length() + 1, socketId);
}

int NetworkManager::receive(uint32_t socketId, void* data, uint16_t maxSize)
{
	int bytesRead = SDLNet_TCP_Recv(sockets.at(socketId), data, maxSize);
	if (bytesRead <= 0)
		std::cout << "SDLNet_TCP_RECV: " << SDLNet_GetError() << std::endl;

	/*int totalBytesRead = 0;
	int lastBytesRead;
	do {
		lastBytesRead = SDLNet_TCP_Recv(sockets.at(socketId), data, maxSize);
		if (lastBytesRead <= 0)
			printf("SDLNet_TCP_RECV: %s\n", SDLNet_GetError());
		totalBytesRead += lastBytesRead;
	} while (lastBytesRead > 0 && totalBytesRead < maxSize);*/

	return bytesRead;
}

void NetworkManager::close(uint32_t socketId)
{
	SDLNet_TCP_DelSocket(socketSet, sockets.at(socketId));
	SDLNet_TCP_Close(sockets.at(socketId));
}

void NetworkManager::quit()
{
	SDLNet_FreeSocketSet(socketSet);
	for (socket_map::iterator it = sockets.begin(); it != sockets.end(); ++it)
		SDLNet_TCP_Close(it->second);
	sockets.clear();
	SDLNet_Quit();
}

uint32_t NetworkManager::addSocket(TCPsocket socket)
{
	if (SDLNet_TCP_AddSocket(socketSet, socket) == -1) {
		std::cout << "SDLNet_TCP_AddSocket: " << SDLNet_GetError() << std::endl;
		return BAD_SOCKET_ID;
	}

	uint32_t i = 0;
	while (!sockets.emplace(i++, socket).second) {}
	return i-1;
}

std::string printIP(uint32_t ip)
{
	std::stringstream addr_ss;
	for (int i = 0; i < 4; i++) {
		addr_ss << (ip >> 8*i) % 256;
		if (i < 3) addr_ss << ".";
	}
	return addr_ss.str();
}

}