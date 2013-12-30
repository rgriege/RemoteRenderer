#include "ServerApp.h"
#include "ConcurrentQueue.h"
#include "Server.h"

extern ConcurrentQueue<AVPacket*> packet_queue;

bool ServerApp::run()
{
    if (!_initOgre())
        return false;

    _createScene();

    Server::getInstance();
    window_stream = new GLWindowStream();
#pragma push_macro("PixelFormat")
#undef PixelFormat
    buffer = Ogre::PixelBox(renderWnd->getWidth(), renderWnd->getHeight(), 1,
        Ogre::PixelFormat::PF_B8G8R8, malloc(renderWnd->getWidth()*renderWnd->getHeight()*3));
#pragma pop_macro("PixelFormat")

    /* ffmpeg init */
    encoder = new Encoder();
    if (!encoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, renderWnd->getWidth(), renderWnd->getHeight(), 25))
    //if (!encoder->bootstrap(AV_CODEC_ID_H264, width, height, 25))
        exit(EXIT_FAILURE);

    bool shutdown = false;
    int timeSinceLastFrame = 1;
    int startTime = 0;
    while(!shutdown)
    {
        if(renderWnd->isClosed())
            shutdown = true;

        Ogre::WindowEventUtilities::messagePump();

        if(renderWnd->isActive())
        {
            startTime = timer->getMillisecondsCPU();
            root->renderOneFrame();
            timeSinceLastFrame = timer->getMillisecondsCPU() - startTime;
        }
        else
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
    }

    free(buffer.data);
    delete window_stream;
    delete encoder;

    return true;
}

bool ServerApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    cameraNode->yaw(Ogre::Radian(0.0003f));

    renderWnd->copyContentsToMemory(buffer, Ogre::RenderTarget::FrameBuffer::FB_AUTO);

    // create the packet
    AVPacket* pkt = new AVPacket();
    av_init_packet(pkt);
    encoder->write_rgb_data_to_frame(static_cast<uint8_t*>(buffer.data));
    encoder->encode_frame(*pkt);
    packet_queue.push(pkt);

    return true;
}

bool ServerApp::_initOgre()
{
    new Ogre::LogManager();
    ogreLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
    ogreLog->setDebugOutputEnabled(true);

    root = new Ogre::Root();

    // set up resources
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load("E:\\Documents\\Ogre\\RemoteRenderer\\ProofOfConcept\\Debug\\resources.cfg");
 
    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
 
    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

    if(!root->showConfigDialog())
        return false;
    renderWnd = root->initialise(true, "Ogre Remote Renderer");
    renderWnd->resize(300, 300);

    timer = new Ogre::Timer();
    timer->reset();

    renderWnd->setActive(true);

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}

void ServerApp::_createScene()
{
    sceneMgr = root->createSceneManager(Ogre::ST_GENERIC);

    Ogre::Entity* ogreHead = sceneMgr->createEntity("Head", "ogrehead.mesh");
    Ogre::SceneNode* headNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    headNode->attachObject(ogreHead);

    Ogre::Light* light = sceneMgr->createLight();
    light->setPosition(20, 80, 50);
    sceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

    camera = sceneMgr->createCamera("MainCamera");
    camera->setNearClipDistance(5);
    camera->setPosition(0, 0, 80);
    camera->setAutoTracking(true, headNode);
    cameraNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
    cameraNode->attachObject(camera);

    viewport = renderWnd->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 0));
    camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

    root->addFrameListener(this);
}