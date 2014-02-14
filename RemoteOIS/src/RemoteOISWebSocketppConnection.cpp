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

#include "RemoteOISWebSocketppConnection.h"

using namespace RemoteOIS;

//---------------------------------------------------------------------------//
WebSocketppConnection::WebSocketppConnection(connection_ptr connection)
    : mConnection(connection)
{
    mConnection->set_open_handler(
        bind(&WebSocketppConnection::_onOpen, this, _1));
    mConnection->set_message_handler(
        bind(&WebSocketppConnection::_onMessage, this, _1, _2));
    mConnection->set_close_handler(
        bind(&WebSocketppConnection::_onClose, this, _1));
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::open()
{
    // mConnection->start();
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::send(WindowDataRequest request) const
{
    mConnection->send(request.data, request.len, 
                      websocketpp::frame::opcode::binary);
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::addConnectionListener(ConnectionListener* listener)
{
    mConnectionListeners.insert(listener);
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::removeConnectionListener(ConnectionListener* listener)
{
    message_listener_set::iterator it = mConnectionListeners.find(listener);
    if (it != mConnectionListeners.end())
        mConnectionListeners.erase(it);
    else
        throw std::out_of_range("No such listener");
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::close()
{
    mConnection->close(websocketpp::close::status::normal, "closed by user");
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::_onOpen(connection_hdl hdl)
{
    /*message_listener_set::iterator it  = mConnectionListeners.begin(),
                                   end = mConnectionListeners.end();
    for ( ; it != end; ++it)
        (*it)->onOpen();*/
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::_onMessage(connection_hdl hdl, message_ptr msg)
{
    WindowDataResponse response = { 
        reinterpret_cast<const void*>(msg->get_raw_payload().c_str()),
        msg->get_payload().length()
    };
    message_listener_set::iterator it = mConnectionListeners.begin(),
                                   end = mConnectionListeners.end();
    for ( ; it != end; ++it) {
        ConnectionListener* const& listener = *it;
        if (listener->understands(response))
            listener->interpret(response);
    }
}

//---------------------------------------------------------------------------//
void WebSocketppConnection::_onClose(connection_hdl hdl)
{
    /*message_listener_set::iterator it = mConnectionListeners.begin(),
                                   end = mConnectionListeners.end();
    for ( ; it != end; ++it)
        (*it)->onClose();*/
}