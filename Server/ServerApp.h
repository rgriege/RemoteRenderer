#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/smart_ptr/owner_less.hpp>
#include <mutex>
#include <atomic>
#include "Config.h"

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

class Server
{
public:
    Server();

    bool run();

private:
    typedef std::set<connection_hdl,boost::owner_less<connection_hdl>> con_list;

	void _loadConfig();
    void _initServer();

    void _onOpen(connection_hdl hdl);
    void _onMessage(connection_hdl hdl, server::message_ptr msg);
    void _onClose(connection_hdl hdl);
    
    Config mConfig;
    server mServer;
	con_list mConnections;
    uint16_t mNextPort;
};

#endif