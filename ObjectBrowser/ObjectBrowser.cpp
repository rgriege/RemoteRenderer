#include "ObjectBrowser.h"

#include <stdlib.h>
#include <time.h>

#include "GameState.h"

ObjectBrowser::ObjectBrowser(std::string remoteHost, uint16_t remotePort, uint16_t localPort)
    : mRemoteHost(remoteHost), mRemotePort(remotePort), mLocalPort(localPort)
{
    mAppStateManager = 0;
}

ObjectBrowser::~ObjectBrowser()
{
    delete mAppStateManager;
    delete OgreFramework::getSingletonPtr();
}

void ObjectBrowser::startGame()
{
    new OgreFramework(mRemoteHost, mRemotePort, mLocalPort);
    Location loc = (mRemoteHost != "" && mRemotePort != 0 && mLocalPort != 0) ? REMOTE : LOCAL;
    if(!OgreFramework::getSingletonPtr()->initOgre("RemoteOgreFramework", 0, 0, loc, loc))
        return;

    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

    mAppStateManager = new AppStateManager();
    GameState::create(mAppStateManager, "GameState");

    srand(time(NULL));

    mAppStateManager->start(mAppStateManager->findByName("GameState"));
}

#undef main
int main(int argc, char **argv)
{
    ObjectBrowser* app;
    if (argc >= 4) {
        std::string remoteHost = argv[1];
        uint16_t remotePort = atoi(argv[2]);
        uint16_t localPort = atoi(argv[3]);
        app = new ObjectBrowser(remoteHost, remotePort, localPort);
    } else {
        app = new ObjectBrowser();
    }

    app->startGame();

    delete app;
    return 0;
}