#ifndef _CLIENT_APP_H
#define _CLIENT_APP_H

#include <string>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::client<websocketpp::config::asio_client>::message_ptr message_ptr;

using websocketpp::connection_hdl;
using websocketpp::lib::bind;

class ClientApp
{
public:
    ClientApp();
    bool run();

protected:
    virtual void _onMessage(client* client, connection_hdl hdl, message_ptr msg) = 0;
    virtual std::string _getInitialRequest() = 0;

private:
    void _initConnection();
    void _onOpen(client* client, connection_hdl hdl);
};

#endif // _CLIENT_APP_H
