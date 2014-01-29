#include "RemoteOISConnectionManager.h"
#include "OISException.h"

using namespace RemoteOIS;

ConnectionManager* ConnectionManager::sInstance = 0;

ConnectionManager* ConnectionManager::getInstance()
{
	if (!sInstance)
		sInstance = new ConnectionManager();
	return sInstance;
}

Connection* ConnectionManager::find(const std::string& name)
{
	connection_map::iterator it = mConnections.find(name);
	return it != mConnections.end() ? it->second : NULL;
}

void ConnectionManager::add(Connection* connection, const std::string& name)
{
	if (mConnections.find(name) != mConnections.end())
		OIS_EXCEPT(OIS::E_Duplicate, "A connection with that name already exists");
	mConnections[name] = connection;
}