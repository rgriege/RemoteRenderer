#ifndef __SERVER_H__
#define __SERVER_H__

#include <string>
#include <vector>

#include "NetworkManager.h"
#include "constants.h"
#include "SDL_thread.h"

#define MAX_HOSTS 4

typedef std::vector<uint32_t> socketIdVector;

struct host {
	uint32_t socketId;
	IPaddress addr;
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
	void stop();

private:
	void start();
	void addClient();
	void handleClient(uint32_t socketId);
	void removeClient(uint32_t socketId);

	friend int createServer(void *prt);

	NeTwerk::NetworkManager* netMgr;
	uint32_t listenSocketId;
	socketIdVector clientSocketIds;

	static Server* instance;
	static SDL_Thread* thread;
	static bool quit;
};

#endif // __SERVER_H__