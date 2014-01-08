/*
The zlib/libpng License

Copyright (c) 2014 Ryan Griege (www.github.com/rgriege)

This software is provided 'as-is', without any express or implied warranty. In no event will
the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial 
applications, and to alter it and redistribute it freely, subject to the following
restrictions:

    1. The origin of this software must not be misrepresented; you must not claim that 
		you wrote the original software. If you use this software in a product, 
		an acknowledgment in the product documentation would be appreciated but is 
		not required.

    2. Altered source versions must be plainly marked as such, and must not be 
		misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#ifndef OIS_RemoteConnectionManager_H
#define OIS_RemoteConnectionManager_H

#include <string>
#include <map>

#include "remote/RemotePrereqs.h"

namespace OIS
{
	class RemoteConnectionManager
	{
		typedef std::map<std::string, RemoteConnection*> connection_map;

	public:
		static RemoteConnectionManager* getInstance();
		RemoteConnection* find(const std::string& name);

	private:
		RemoteConnectionManager() {}
		RemoteConnectionManager(const RemoteConnectionManager&);
		RemoteConnectionManager& operator=(const RemoteConnectionManager&);
		
		static RemoteConnectionManager* sInstance;
		connection_map mConnections;
	};
}

#endif