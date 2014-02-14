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

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <set>

#include "RemoteOISPrereqs.h"

using websocketpp::connection_hdl;

namespace RemoteOIS
{
    class _RemoteOISExport WebSocketppConnection : public Connection
    {
        typedef websocketpp::config::asio asio_config;
        typedef websocketpp::server<asio_config> server;
        typedef websocketpp::endpoint<websocketpp::connection<asio_config>,
            asio_config>::connection_ptr connection_ptr;
        typedef server::message_ptr message_ptr;
        typedef std::set<ConnectionListener*> message_listener_set;

    public:
        WebSocketppConnection(connection_ptr connection);

        /** @copydoc Connection::open */
        virtual void open();

        /** @copydoc Connection::send */
        virtual void send(WindowDataRequest) const;

        /** @copydoc Connection::addConnectionListener */
        virtual void addConnectionListener(ConnectionListener*);

        /** @copydoc Connection::removeConnectionListener */
        virtual void removeConnectionListener(ConnectionListener*);

        /** @copydoc Connection::close */
        virtual void close();

    protected:
        //! Internal websocketpp open handler
        virtual void _onOpen(connection_hdl hdl);

        //! Internal websocketpp message handler
        virtual void _onMessage(connection_hdl hdl, message_ptr msg);

        //! Internal websocketpp close handler
        virtual void _onClose(connection_hdl hdl);

        //! Set of listeners that have been added
        std::set<ConnectionListener*> mConnectionListeners;

        //! Internal websocketpp connection handle
        connection_ptr mConnection;
    };
}