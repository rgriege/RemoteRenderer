#ifndef __NetworkManager_h__
#define __NetworkManager_h__

#include <string>
#include <map>
#include <stdint.h>

#include "SDL_net.h"

#define DEFAULT_PORT 2000
#define LISTEN_PORT 53255
#define BAD_SOCKET_ID UINT32_MAX

#ifdef NETWERK_EXPORT
    #define NETWERK_API __declspec(dllexport)
#else
    #define NETWERK_API __declspec(dllimport)
#endif

namespace NeTwerk {

	class NETWERK_API NetworkManager
	{
	private:
		NetworkManager();
		NetworkManager(NetworkManager const&);
		NetworkManager& operator=(NetworkManager const&);

		uint32_t addSocket(TCPsocket socket);

	public:
		static NetworkManager* getInstance();
		static std::string getLocalHostname();
		static std::string getLocalIP();

		std::string getIP(uint32_t socketId);

		uint32_t listen(uint16_t port = DEFAULT_PORT);
		uint32_t accept(uint32_t listenSocketId);
		uint32_t connect(const char* host, uint16_t port = DEFAULT_PORT);
		uint32_t connect(uint32_t host, uint16_t port);

		bool prime(uint32_t timeout = 0);
		bool check(uint32_t socketId);
		void send(void* data, int size, uint32_t socketId);
		void send(std::string message, uint32_t sockedId);
		int receive(uint32_t socketId, void* data, uint16_t maxSize);

		void close(uint32_t socketId);
		void quit();

	private:
		static NetworkManager* instance;
		SDLNet_SocketSet socketSet;
		//uint32_t socketSetSize;
		typedef std::map<uint32_t, TCPsocket> socket_map;
#pragma warning(disable : 4251)
		socket_map sockets;
#pragma warning(default : 4251)
	};

	std::string printIP(uint32_t ip);

}

#endif
