#include <thread>

#include "ServerApp.h"
#include "ConcurrentQueue.h"

extern ConcurrentQueue<AVPacket*> packet_queue;

bool ServerApp::run()
{
    if (!_initOgre())
        return false;

    _createScene();

	_initOis();

#pragma push_macro("PixelFormat")
#undef PixelFormat
    buffer = Ogre::PixelBox(renderWnd->getWidth(), renderWnd->getHeight(), 1,
        Ogre::PixelFormat::PF_B8G8R8, malloc(renderWnd->getWidth()*renderWnd->getHeight()*3));
#pragma pop_macro("PixelFormat")

    /* ffmpeg init, also try AV_CODEC_ID_H264 */
    encoder = new Encoder();
    if (!encoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, renderWnd->getWidth(), renderWnd->getHeight(), frameRate))
        exit(EXIT_FAILURE);

    _initServer();
    std::thread t(std::bind(&server::run, &mServer));

	bool shutdown = false;
    int startTime = 0;
    while(!shutdown)
    {
        if(renderWnd->isClosed())
            shutdown = true;

        Ogre::WindowEventUtilities::messagePump();

        if(renderWnd->isActive())
        {
            startTime = timer->getMillisecondsCPU();
			if (!mConnections.empty()) {
				root->renderOneFrame();
				mKeyboard->capture();
				mMouse->capture();
			}
            timeSinceLastFrame = timer->getMillisecondsCPU() - startTime;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(std::max(frameTime - timeSinceLastFrame, 0));
#else
            //sleep(1);
#endif
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
    delete encoder;

    return true;
}

bool ServerApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    cameraNode->yaw(Ogre::Radian(angularVelocity * std::max(timeSinceLastFrame, frameTime)));

    renderWnd->copyContentsToMemory(buffer, Ogre::RenderTarget::FrameBuffer::FB_AUTO);

    // create the packet
    AVPacket* pkt = new AVPacket();
    av_init_packet(pkt);
    encoder->write_rgb_data_to_frame(static_cast<uint8_t*>(buffer.data));
    encoder->encode_frame(*pkt);
    for (auto it : mConnections) {
		try {
			mServer.send(it, pkt->data, pkt->size, websocketpp::frame::opcode::binary);
		} catch (websocketpp::lib::error_code&) {}
	}
    av_free_packet(pkt);
    //packet_queue.push(pkt);

    return true;
}

bool ServerApp::mouseMoved( const OIS::MouseEvent &arg )
{
	//std::cout << "Mouse Moved!" << std::endl;
	return true;
}

bool ServerApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	std::cout << "Mouse Pressed!" << std::endl;
	angularVelocity *= -1;
	return true;
}

bool ServerApp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	std::cout << "Mouse Released!" << std::endl;
	angularVelocity *= -1;
	return true;
}

bool ServerApp::keyPressed(const OIS::KeyEvent &arg)
{
	std::cout << "Key Pressed!" << std::endl;
	return true;
}

bool ServerApp::keyReleased(const OIS::KeyEvent &arg)
{
	std::cout << "Key Released!" << std::endl;
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

    if(!root->restoreConfig()) {
        if (root->showConfigDialog())
            root->saveConfig();
        else
            return false;
    }
    renderWnd = root->initialise(true, "Ogre Remote Renderer");
    renderWnd->resize(300, 300);

    timer = new Ogre::Timer();
    timer->reset();

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    renderWnd->setActive(true);

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

void ServerApp::_initServer()
{
    mServer.init_asio();
    mServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
    mServer.clear_access_channels(websocketpp::log::alevel::frame_header);
    mServer.set_open_handler(boost::bind(&ServerApp::_onOpen, this, _1));
    mServer.set_close_handler(boost::bind(&ServerApp::_onClose, this, _1));
    mServer.listen(9002);
    mServer.start_accept();
}

void ServerApp::_initOis()
{
	size_t hWnd = 0;
	renderWnd->getCustomAttribute("WINDOW", &hWnd);
	mInputMgr = OIS::InputManager::createInputSystem(hWnd);

	mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));
	mMouse->setEventCallback(this);
	mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
	mKeyboard->setEventCallback(this);
}

void ServerApp::_onOpen(connection_hdl hdl)
{
    std::cout << "socket opened!" << std::endl;
    std::lock_guard<std::mutex> lock(mMutex);
    mConnections.insert(hdl);

	uint16_t first_message[4];
	memcpy(first_message, "jsmp", 4);
	first_message[2] = htons(300);
	first_message[3] = htons(300);
	mServer.send(hdl, first_message, 8, websocketpp::frame::opcode::binary);
}

void ServerApp::_onClose(connection_hdl hdl)
{
    std::cout << "socket closed!" << std::endl;
    std::lock_guard<std::mutex> lock(mMutex);
    mConnections.erase(hdl);
}