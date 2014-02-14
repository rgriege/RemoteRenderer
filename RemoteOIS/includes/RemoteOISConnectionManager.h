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

#ifndef _REMOTE_CONNECTION_MANAGER_H
#define _REMOTE_CONNECTION_MANAGER_H

#include <string>
#include <map>

#include "RemoteOISPrereqs.h"

namespace RemoteOIS
{
    /**
        Singleton class to used for passing connections by a string tag to
        InputManager::createInputSystem( OIS::ParamList &paramList );
    */
    class _RemoteOISExport ConnectionManager
    {
        typedef std::map<std::string, Connection*> connection_map;

    public:
        /**
        @remarks
            Singleton access function
        @return
            An instance of the Connection Manager
        */
        static ConnectionManager* getInstance();

        /**
        @remarks
            Searches for a Connection by name
        @param name
            The name given to the connection when add was called
        @return
            A pointer to the Connection, or NULL if none was found
        */
        Connection* find(const std::string& name);

        /**
        @remarks
            Adds a Connection by name
        @param connection
            The Connection to add
        @param name
            The name of the Connection
        */
        void add(Connection* connection, const std::string& name);

    private:
        /**
        @remarks
            Hidden so this class cannot be instanced with default constructor
        */
        ConnectionManager() {}

        /**
        @remarks
            Hidden so this class cannot be instanced with copy constructor
        */
        ConnectionManager(const ConnectionManager&);

        /**
        @remarks
            Hidden so this class cannot be instanced with copy assignment operator
        */
        ConnectionManager& operator=(const ConnectionManager&);

        //! Instance of the singleton
        static ConnectionManager* sInstance;

        //! Name-mapped of Connection pointers
        connection_map mConnections;
    };
}

#endif