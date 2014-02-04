#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include <OGRE\Ogre.h>
#include <OIS.h>
#include <RemoteOIS.h>
#include <RemoteOISWebSocketppConnection.h>
#include <Encoder.h>
#include <rgbData.h>
#include <thread>

extern "C" {
#include <libavcodec\avcodec.h>
}
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <boost/smart_ptr/owner_less.hpp>
#include <mutex>
#include <condition_variable>
#include <atomic>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;

class ObjectBrowser : public Ogre::FrameListener, public OIS::MouseListener, public OIS::KeyListener
{
public:
    ObjectBrowser(char* remoteHost, char* remotePort, char* localPort, int fps = 30);

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
    void _createScene(bool local);
    void _initServer();
    void _closeServer();
	void _initOis(bool local);

    void _onRenderOpen(connection_hdl hdl);
    void _onInputOpen(connection_hdl hdl);
    void _onClose(connection_hdl hdl);

    Ogre::Log* ogreLog;
    Ogre::Root* root;
    Ogre::RenderWindow* renderWnd;
	Ogre::RenderTexture* renderTex;
    Ogre::Viewport* viewport;
    Ogre::Timer* timer;
    Ogre::Camera* camera;
    Ogre::SceneNode* cameraNode;
	Ogre::SceneNode* headNode;
    Ogre::SceneManager* sceneMgr;
    Encoder* encoder;
    Ogre::PixelBox buffer;
    const int frameRate;
    const int frameTime;
    int timeSinceLastFrame;
	std::atomic_bool mShutdown;

	OIS::InputManager* mLocalInputMgr;
	OIS::InputManager* mRemoteInputMgr;
	OIS::Mouse* mMouse;
	OIS::Keyboard* mKeyboard;

	const Ogre::Real mZoomScale;
	Ogre::Vector3 mMouseDownPos;
	Ogre::Quaternion mMouseDownOrientation;
	Ogre::Vector3 mMouseDownScale;
	bool rotating;
	bool zooming;

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
};

#endif