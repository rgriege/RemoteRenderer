#include "LibraryApp.h"
#include "Config.h"

void LibraryApp::_onMessage(client* client, connection_hdl hdl, message_ptr msg)
{
    Config config;
    std::stringstream stream(msg->get_raw_payload());
    config.parse(stream);
    config.start();
    while (config.hasMoreGames())
        std::cout << " - " << config.getNextGame().name << std::endl;
    client->stop();
}

std::string LibraryApp::_getInitialRequest()
{
    return "list";
}
