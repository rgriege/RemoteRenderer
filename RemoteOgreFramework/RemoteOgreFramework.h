#ifndef REMOTE_OGRE_FRAMEWORK_H
#define REMOTE_OGRE_FRAMEWORK_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include <OGRE\Ogre.h>
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

#include <OGRE\SdkTrays.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

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
    OgreFramework(char* remoteHost = NULL, char* remotePort = NULL, char* localPort = NULL);
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
    CEGUI::OgreRenderer*        m_pGUIRenderer;
    CEGUI::System*              m_pGUISystem;

private:
    OgreFramework(const OgreFramework&);
    OgreFramework& operator= (const OgreFramework&);

    void _onRenderOpen(connection_hdl hdl);
    void _onInputOpen(connection_hdl hdl);
    void _onClose(connection_hdl hdl);

    OIS::MouseState scaleMouseState(const OIS::MouseState& state);

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

	std::atomic_int mConnections;

    const std::string mRemoteHost;
    const uint16_t mRemotePort;
    const uint16_t mLocalPort;

    std::atomic_bool mShutdown;
};

#endif