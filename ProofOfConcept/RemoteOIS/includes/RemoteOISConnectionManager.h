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
#ifndef RemoteOIS_ConnectionManager_H
#define RemoteOIS_ConnectionManager_H

#include <string>
#include <map>

#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
	class _RemoteOISExport ConnectionManager
	{
		typedef std::map<std::string, Connection*> connection_map;

	public:
		static ConnectionManager* getInstance();
		Connection* find(const std::string& name);
		void add(Connection* connection, const std::string& name);

	private:
		ConnectionManager() {}
		ConnectionManager(const ConnectionManager&);
		ConnectionManager& operator=(const ConnectionManager&);
		
		static ConnectionManager* sInstance;
		connection_map mConnections;
	};
}

#endif