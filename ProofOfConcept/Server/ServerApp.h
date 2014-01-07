#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include <OGRE\Ogre.h>
#include <OIS.h>
#include <Encoder.h>
#include <rgbData.h>
extern "C" {
#include <libavcodec\avcodec.h>
}
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/smart_ptr/owner_less.hpp>
#include <mutex>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

class ServerApp : public Ogre::FrameListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
    ServerApp(int fps = 30)
		: frameRate(fps),
		frameTime(1000/frameRate),
		angularVelocity(1.f/8000 * Ogre::Math::TWO_PI) {}

    bool run();

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

private:
    typedef std::set<connection_hdl,boost::owner_less<connection_hdl>> con_list;

    bool _initOgre();
    void _createScene();
    void _initServer();
	void _initOis();

    void _onOpen(connection_hdl hdl);
    void _onClose(connection_hdl hdl);

    Ogre::Log* ogreLog;
    Ogre::Root* root;
    Ogre::RenderWindow* renderWnd;
    Ogre::Viewport* viewport;
    Ogre::Timer* timer;
    Ogre::Camera* camera;
    Ogre::SceneNode* cameraNode;
    Ogre::SceneManager* sceneMgr;
    Encoder* encoder;
    Ogre::PixelBox buffer;
    const int frameRate;
    const int frameTime;
    int timeSinceLastFrame;
	Ogre::Real angularVelocity;

	OIS::InputManager* mInputMgr;
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;

    server mServer;
    con_list mConnections;
    std::mutex mMutex;
};

#endif