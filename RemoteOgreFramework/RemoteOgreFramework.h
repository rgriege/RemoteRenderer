#ifndef REMOTE_OGRE_FRAMEWORK_H
#define REMOTE_OGRE_FRAMEWORK_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <Ogre.h>
#include <OIS.h>
#include <RemoteOIS.h>
#include <Encoder.h>
#include <rgbData.h>
extern "C" {
#include <libavcodec\avcodec.h>
}
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/smart_ptr/owner_less.hpp>

#include <SdkTrays.h>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

enum Location
{
    LOCAL = 0x1,
    REMOTE = 0x2
};

class OgreFramework : public Ogre::Singleton<OgreFramework>,
    public Ogre::FrameListener, Ogre::WindowEventListener,
    public OIS::MouseListener, public OIS::KeyListener
{
public:
    OgreFramework(std::string remoteHost = "", uint16_t remotePort = 0, uint16_t localPort = 0);
    ~OgreFramework();

    bool initOgre(Ogre::String wndTitle,
        OIS::KeyListener* pKeyListener = 0,
        OIS::MouseListener* pMouseListener = 0,
        Location renderLocation = LOCAL,
        Location inputLocation = LOCAL);
    void updateOgre(double timeSinceLastFrame);
    bool closed();

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

    virtual void windowResized(Ogre::RenderWindow* rw);
    virtual void windowClosed(Ogre::RenderWindow* rw);

    virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);

    Ogre::Root*             m_pRoot;
    Ogre::RenderTarget*     m_pRenderWnd;
    Ogre::Viewport*         m_pViewport;
    Ogre::Log*              m_pLog;
    Ogre::Timer*            m_pTimer;
    Ogre::String            mResourcesCfg;
    Ogre::String            mPluginsCfg;

    OIS::InputManager*      m_pInputMgr;
    OIS::Keyboard*          m_pKeyboard;
    OIS::Mouse*             m_pMouse;

    OgreBites::SdkTrayManager*	m_pTrayMgr;

private:
    OgreFramework(const OgreFramework&);
    OgreFramework& operator= (const OgreFramework&);

    void _onRenderOpen(connection_hdl hdl);
    void _onInputOpen(connection_hdl hdl);
    void _onClose(connection_hdl hdl);

    Location mInputLocation;
    Location mRenderLocation;

    Encoder* mEncoder;
    Ogre::PixelBox mBuffer;

	server mInputServer;
    std::thread* mInputThread;
	connection_hdl mInputHdl;
	std::mutex mInputConMtx;
	std::condition_variable mInputConCv;

    server mRenderServer;
    std::thread* mRenderThread;
	connection_hdl mRenderHdl;
    std::mutex mRenderConMtx;
    std::condition_variable mRenderConCv;

	std::atomic_int mConnections;

    const std::string mRemoteHost;
    const uint16_t mRemotePort;
    const uint16_t mLocalPort;

    std::atomic_bool mShutdown;
};

#endif