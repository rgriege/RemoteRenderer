#include "ClientApp.h"

ClientApp::ClientApp()
{
}

bool ClientApp::run()
{
    _initConnection();
}

void ClientApp::_onOpen(client* client, connection_hdl hdl)
{
    client->send(hdl, _getInitialRequest(), websocketpp::frame::opcode::text);
}

void ClientApp::_initConnection()
{
    client client;
    client.init_asio();
    client.clear_access_channels(websocketpp::log::alevel::all);
    client.set_open_handler(bind(&ClientApp::_onOpen, this, &client, _1));
    client.set_message_handler(bind(&ClientApp::_onMessage, this, &client, _1, _2));
    websocketpp::lib::error_code ec;
    client::connection_ptr con = client.get_connection("ws://localhost:9001", ec);
    client.connect(con);
    client.run();
}

