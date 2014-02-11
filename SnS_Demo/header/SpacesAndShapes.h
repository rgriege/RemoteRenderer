#ifndef __SpacesAndShapes_h__
#define __SpacesAndShapes_h__

#include <RemoteOgreFramework.h>
#include <AppStateManager.h>

class SpacesAndShapes
{
	public:
		SpacesAndShapes(std::string remoteHost = "", uint16_t remotePort = 0, uint16_t localPort = 0);
		virtual ~SpacesAndShapes();

		void startGame();
	private:
		AppStateManager* mAppStateManager;
        std::string mRemoteHost;
        uint16_t mRemotePort;
        uint16_t mLocalPort;
};

#endif
