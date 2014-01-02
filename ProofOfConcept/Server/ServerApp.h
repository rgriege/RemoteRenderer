#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include <OGRE\Ogre.h>
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

class ServerApp : public Ogre::FrameListener
{
public:
    ServerApp(int fps = 30) : frameRate(fps), frameTime(1000/frameRate) {}
    bool run();

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

private:
    typedef std::set<connection_hdl,boost::owner_less<connection_hdl>> con_list;

    bool _initOgre();
    void _createScene();
    void _initServer();

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

    server mServer;
    con_list mConnections;
    std::mutex mMutex;
};

#endif