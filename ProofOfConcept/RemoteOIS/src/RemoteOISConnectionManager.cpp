#include "RemoteConnectionManager.h"

using namespace OIS;

RemoteConnectionManager* RemoteConnectionManager::sInstance = 0;

RemoteConnectionManager* RemoteConnectionManager::getInstance()
{
	if (!sInstance)
		sInstance = new RemoteConnectionManager();
	return sInstance;
}

RemoteConnection* RemoteConnectionManager::find(const std::string& name)
{
	connection_map::iterator it = mConnections.find(name);
	return it != mConnections.end() ? it->second : NULL;
}