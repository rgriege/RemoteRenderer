#ifndef __NetworkManager_h__
#define __NetworkManager_h__

#include <string>

#include "SDL_net.h"

#undef SendMessage
#define DEFAULT_PORT 2000

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

	public:
		static NetworkManager* GetInstance();
		static std::string GetHostname();
		std::string GetIP();
		void ConnectToServer(std::string host);
		void SendMessage(std::string msg, TCPsocket sd = NULL);
		void SendMessage(char* msg, TCPsocket sd = NULL);
		bool CheckSocket();
		std::string ReceiveMessage();
		void Close();

	private:
		static NetworkManager* instance;
		IPaddress ip;
		TCPsocket serversocket;
		SDLNet_SocketSet socketset;
	};

}

#endif
