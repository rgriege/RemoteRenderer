#ifndef _LIBRARY_APP_H
#define _LIBRARY_APP_H

#include "ClientApp.h"

class LibraryApp : public ClientApp
{
protected:
    virtual void _onMessage(client* client, connection_hdl hdl, message_ptr msg);
    virtual std::string _getInitialRequest();
};

#endif // _LIBRARY_APP_H
