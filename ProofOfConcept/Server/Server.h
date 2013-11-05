#ifndef __SERVER_H__
#define __SERVER_H__

#include "SDL_net.h"
#include "constants.h"
#include <string>
#include "SDL_thread.h"
#include "NetworkManager.h"

#define MAX_CLIENTS 2

struct client {
	uint32_t socketId;
	IPaddress ip;
	std::string name;
};

class Server
{
private:
	Server();
	Server(Server const&);
	Server& operator=(Server const&);

public:
	static Server* getInstance();
	void waitForConnection();
	void broadcast(void* data, int size);
	std::string getHostname();

private:
	void addClient();
	void handleClient(int idx);
	void removeClient(int idx);

	//friend int createServer(void *prt);

	static Server* instance;
	//static SDL_Thread* thread;
	static uint32_t serversocket;
	static client clients[MAX_CLIENTS];
	static std::string hostname;
	static bool quit;
	static NeTwerk::NetworkManager* netMgr;
};

#endif // __SERVER_H__