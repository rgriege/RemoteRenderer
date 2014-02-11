#include "SpacesAndShapes.h"

#include <stdlib.h>
#include <time.h>

#include "MenuState.h"
#include "CharacterSelectState.h"
#include "CharacterCreateState.h"
#include "GameState.h"

SpacesAndShapes::SpacesAndShapes(std::string remoteHost, uint16_t remotePort, uint16_t localPort)
    : mRemoteHost(remoteHost), mRemotePort(remotePort), mLocalPort(localPort)
{
	mAppStateManager = 0;
}

SpacesAndShapes::~SpacesAndShapes()
{
	delete mAppStateManager;
	delete OgreFramework::getSingletonPtr();
}

void SpacesAndShapes::startGame()
{
    new OgreFramework(mRemoteHost, mRemotePort, mLocalPort);
    Location loc = (mRemoteHost != "" && mRemotePort != 0 && mLocalPort != 0) ? REMOTE : LOCAL;
	if(!OgreFramework::getSingletonPtr()->initOgre("RemoteOgreFramework", 0, 0, loc, loc))
		return;

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

	mAppStateManager = new AppStateManager();
	MenuState::create(mAppStateManager, "MenuState");
    CharacterSelectState::create(mAppStateManager, "CharacterSelectState");
    CharacterCreateState::create(mAppStateManager, "CharacterCreateState");
    GameState::create(mAppStateManager, "GameState");
    
    srand(time(NULL));

	mAppStateManager->start(mAppStateManager->findByName("MenuState"));
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <shellapi.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
    {
        int argc;
        LPWSTR *szArgList = CommandLineToArgvW(GetCommandLine(), &argc);
        if (szArgList == NULL)
        {
            MessageBox(NULL, L"Unable to parse command line", L"Error", MB_OK);
            return 10;
        }

        char** argv = new char*[argc];
        for(int i = 0; i < argc; i++)
        {
            char* arg = new char[50];
            wcstombs(arg, szArgList[i], 50);
            argv[i] = arg;
        }
 
        LocalFree(szArgList);
#else
    int main(int argc, char *argv[])
    {
#endif
        // Create application object
        SpacesAndShapes* app;
        if (argc >= 4) {
            std::string remoteHost = argv[1];
            uint16_t remotePort = atoi(argv[2]);
            uint16_t localPort = atoi(argv[3]);
            app = new SpacesAndShapes(remoteHost, remotePort, localPort);
        } else {
            app = new SpacesAndShapes();
        }

        try {
            app->startGame();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifdef UNICODE
            wchar_t wString[1024];
            MultiByteToWideChar(CP_ACP, 0, e.getFullDescription().c_str(), -1, wString, 1024);
            MessageBox( NULL, wString, L"An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
        delete app;
        return 0;
    }

#ifdef __cplusplus
}
#endif
