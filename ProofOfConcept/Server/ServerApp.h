#ifndef __SERVER_APP_H__
#define __SERVER_APP_H__

#include <OGRE\Ogre.h>
#include <Encoder.h>
#include <rgbData.h>
extern "C" {
#include <libavcodec\avcodec.h>
}
#include "GLWindowStream.h"

class ServerApp : public Ogre::FrameListener
{
public:
    ServerApp(int fps = 30) : frameRate(fps), frameTime(1000/frameRate) {}
    bool run();

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);

private:
    bool _initOgre();
    void _createScene();

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
};

#endif