#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include <RemoteOgreFramework.h>
#include <AppStateManager.h>

class ObjectBrowser
{
public:
    ObjectBrowser(std::string remoteHost = "", uint16_t remotePort = 0, uint16_t localPort = 0);
    virtual ~ObjectBrowser();

    void startGame();

private:
    AppStateManager* mAppStateManager;
    std::string mRemoteHost;
    uint16_t mRemotePort;
    uint16_t mLocalPort;
};

#endif