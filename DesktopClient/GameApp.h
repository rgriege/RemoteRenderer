#ifndef _GAME_APP_H
#define _GAME_APP_H

#include "ClientApp.h"

class GameApp : public ClientApp
{
public:
    GameApp(std::string title);

protected:
    virtual void _onMessage(client* client, connection_hdl hdl, message_ptr msg);
    virtual std::string _getInitialRequest();

private:
    const std::string mTitle;
};

#endif // _GAME_APP_H
