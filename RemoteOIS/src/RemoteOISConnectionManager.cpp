/*
The zlib/libpng License

Copyright (c) 2005-2007 Phillip Castaneda (pjcast -- www.wreckedgames.com)
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

#include "RemoteOISConnectionManager.h"
#include "OISException.h"

using namespace RemoteOIS;

ConnectionManager* ConnectionManager::sInstance = 0;

//---------------------------------------------------------------------------//
ConnectionManager* ConnectionManager::getInstance()
{
    if (!sInstance)
        sInstance = new ConnectionManager();
    return sInstance;
}

//---------------------------------------------------------------------------//
Connection* ConnectionManager::find(const std::string& name)
{
    connection_map::iterator it = mConnections.find(name);
    return it != mConnections.end() ? it->second : NULL;
}

//---------------------------------------------------------------------------//
void ConnectionManager::add(Connection* connection, const std::string& name)
{
    if (mConnections.find(name) != mConnections.end())
        OIS_EXCEPT(OIS::E_Duplicate, "A connection with that name already exists");
    mConnections[name] = connection;
}