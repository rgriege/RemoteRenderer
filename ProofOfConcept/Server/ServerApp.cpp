#include <thread>

#include "ServerApp.h"
#include "ConcurrentQueue.h"

extern ConcurrentQueue<AVPacket*> packet_queue;

#define RENDER_PORT 9002
#define INPUT_PORT 9003

OIS::MouseState scaleMouseState(const OIS::MouseState& state, Ogre::RenderTarget* target)
{
	OIS::MouseState result;
	result.X.abs = (state.X.abs * target->getWidth()) / state.width;
	result.X.rel = (state.X.rel * target->getWidth()) / state.width;
	result.Y.abs = (state.Y.abs * target->getHeight()) / state.height;
	result.Y.rel = (state.Y.rel * target->getHeight()) / state.height;
	result.width = target->getWidth();
	result.height = target->getHeight();
	result.buttons = state.buttons;
	return result;
}

Ogre::Vector3 toSphere(Ogre::RenderTarget* wnd, float x, float y)
{
	float sx = (x - wnd->getWidth()/2)/(wnd->getWidth()/2);
	float sy = -(y - wnd->getHeight()/2)/(wnd->getHeight()/2);
	float pows = pow(sx, 2) + pow(sy, 2);
	return (pows > 1) ? Ogre::Vector3::ZERO : Ogre::Vector3(sx, sy, sqrt(1-pows));
}

ServerApp::ServerApp(int fps)
	: frameRate(fps),
	frameTime(1000/frameRate),
	mShutdown(false),
	mZoomScale(30),
	rotating(false),
	zooming(false)
{
}

bool ServerApp::run()
{
    if (!_initOgre())
        return false;

    _createScene(false);

#pragma push_macro("PixelFormat")
#undef PixelFormat
    buffer = Ogre::PixelBox(renderTex->getWidth(), renderTex->getHeight(), 1,
        Ogre::PixelFormat::PF_B8G8R8, malloc(renderTex->getWidth()*renderTex->getHeight()*3));
#pragma pop_macro("PixelFormat")

    /* ffmpeg init, also try AV_CODEC_ID_H264 */
    encoder = new Encoder();
    if (!encoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, renderTex->getWidth(), renderTex->getHeight(), frameRate))
        exit(EXIT_FAILURE);

	mConnections = 0;
    _initServer();
    std::thread t1(std::bind(&server::run, &mRenderServer));
	std::thread t2(std::bind(&server::run, &mInputServer));

	_initOis(false);

    int startTime = 0;
    while(!mShutdown)
    {
        /*if(renderWnd->isClosed())
            mShutdown = true;*/

        Ogre::WindowEventUtilities::messagePump();

        if(renderTex->isActive())
        {
            startTime = timer->getMillisecondsCPU();
			if (mConnections == 2) {
				mKeyboard->capture();
				mMouse->capture();
				root->renderOneFrame();
			}
            timeSinceLastFrame = timer->getMillisecondsCPU() - startTime;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(std::max(frameTime - timeSinceLastFrame, 0));
#else
            sleep(1);
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

	mRenderServer.stop();
	mInputServer.stop();
	t1.join();
	t2.join();

    return true;
}

bool ServerApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	renderTex->copyContentsToMemory(buffer, Ogre::RenderTarget::FrameBuffer::FB_AUTO);
    //renderWnd->copyContentsToMemory(buffer, Ogre::RenderTarget::FrameBuffer::FB_AUTO);

    // create the packet
    AVPacket* pkt = new AVPacket();
    av_init_packet(pkt);
    encoder->write_rgb_data_to_frame(static_cast<uint8_t*>(buffer.data));
    encoder->encode_frame(*pkt);
	try {
		mRenderServer.send(mRenderHdl, pkt->data, pkt->size, websocketpp::frame::opcode::binary);
	} catch (websocketpp::lib::error_code&) {}
    av_free_packet(pkt);
    //packet_queue.push(pkt);

    return true;
}

bool ServerApp::mouseMoved( const OIS::MouseEvent &arg )
{
	OIS::MouseState state = scaleMouseState(arg.state, renderTex);
	//std::cout << "Mouse Moved!" << std::endl;
	if (rotating) {
		Ogre::Vector3 currentMousePos = toSphere(renderTex, state.X.abs, state.Y.abs);
		Ogre::Vector3 rotationAxis = mMouseDownPos.crossProduct(currentMousePos);
		rotationAxis.normalise();
		Ogre::Radian rotationAngle = Ogre::Math::ACos(
			currentMousePos.dotProduct(mMouseDownPos) /
			(currentMousePos.length()*mMouseDownPos.length()));
		headNode->setOrientation(mMouseDownOrientation);
		headNode->rotate(rotationAxis, rotationAngle, Ogre::Node::TS_WORLD);
	} 
	if (zooming) {
		Ogre::Vector3 currentMousePos = Ogre::Vector3(0, state.Y.abs, 0);
		headNode->setScale(mMouseDownScale + 
			Ogre::Vector3((state.Y.abs - mMouseDownPos.y) / mZoomScale));
	}
	return true;
}

bool ServerApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	OIS::MouseState state = scaleMouseState(arg.state, renderTex);
	std::cout << "Mouse Pressed!" << std::endl;
	switch(id) {
	case OIS::MB_Left:
		mMouseDownPos = toSphere(renderTex, state.X.abs, state.Y.abs);
		mMouseDownOrientation = headNode->getOrientation();
		rotating = true;
		break;
	case OIS::MB_Right:
		mMouseDownPos = Ogre::Vector3(0, state.Y.abs, 0);
		mMouseDownScale = headNode->getScale();
		zooming = true;
		break;
	}
	return true;
}

bool ServerApp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	OIS::MouseState state = scaleMouseState(arg.state, renderTex);
	std::cout << "Mouse Released!" << std::endl;
	switch(id) {
	case OIS::MB_Left:
		rotating = false;
		break;
	case OIS::MB_Right:
		zooming = false;
		break;
	}
	return true;
}

bool ServerApp::keyPressed(const OIS::KeyEvent &arg)
{
	std::cout << "Key Pressed!" << std::endl;
	switch(arg.key) {
	case OIS::KC_R:
		headNode->resetOrientation();
		headNode->setScale(Ogre::Vector3::UNIT_SCALE);
		break;
	case OIS::KC_ESCAPE:
		mShutdown = true;
		break;
	}
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
	//root->initialise(false);
    renderWnd = root->initialise(true, "Ogre Remote Renderer");
    renderWnd->resize(300, 300);

    timer = new Ogre::Timer();
    timer->reset();

    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    return true;
}

void ServerApp::_createScene(bool local)
{
    sceneMgr = root->createSceneManager(Ogre::ST_GENERIC);

    Ogre::Entity* ogreHead = sceneMgr->createEntity("Head", "ogrehead.mesh");
    headNode = sceneMgr->getRootSceneNode()->createChildSceneNode();
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

	Ogre::TexturePtr rtt_texture = Ogre::TextureManager::getSingleton().createManual(
		"RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
		300, 300, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET);
	renderTex = rtt_texture->getBuffer()->getRenderTarget();
	viewport = renderTex->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue::Black);
	camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));

	if (local) {
		viewport = renderWnd->addViewport(camera);
		viewport->setBackgroundColour(Ogre::ColourValue::Black);
		camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
	}

    root->addFrameListener(this);
}

void ServerApp::_initServer()
{
    mRenderServer.init_asio();
    mRenderServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
    mRenderServer.clear_access_channels(websocketpp::log::alevel::frame_header);
    mRenderServer.set_open_handler(boost::bind(&ServerApp::_onOpen, this, _1, true));
    mRenderServer.set_close_handler(boost::bind(&ServerApp::_onClose, this, _1));
    mRenderServer.listen(9002);
    mRenderServer.start_accept();

	mInputServer.init_asio();
    mInputServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
    mInputServer.clear_access_channels(websocketpp::log::alevel::frame_header);
    mInputServer.set_open_handler(boost::bind(&ServerApp::_onOpen, this, _1, false));
    mInputServer.set_close_handler(boost::bind(&ServerApp::_onClose, this, _1));
    mInputServer.listen(9003);
    mInputServer.start_accept();
}

void ServerApp::_initOis(bool local)
{
	if (local) {
		size_t hWnd = 0;
		renderWnd->getCustomAttribute("WINDOW", &hWnd);
		mLocalInputMgr = OIS::InputManager::createInputSystem(hWnd);
	}
	
	std::unique_lock<std::mutex> lk(mInputConMtx);
	mInputConCv.wait(lk, [&]() { return !mInputHdl._empty(); });
	lk.unlock();
	OIS::RemoteConnection* con = new OIS::WebSocketppConnection(mInputServer.get_con_from_hdl(mInputHdl));
	mRemoteInputMgr = OIS::RemoteInputManager::createInputSystem(con);

	//if (remote) {
		mMouse = static_cast<OIS::Mouse*>(mRemoteInputMgr->createInputObject(OIS::OISMouse, true));
		mKeyboard = static_cast<OIS::Keyboard*>(mRemoteInputMgr->createInputObject(OIS::OISKeyboard, true));
	/*} else {
		mMouse = static_cast<OIS::Mouse*>(mLocalInputMgr->createInputObject(OIS::OISMouse, true));
		mKeyboard = static_cast<OIS::Keyboard*>(mLocalInputMgr->createInputObject(OIS::OISKeyboard, true));
	}*/

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);
}

void ServerApp::_onOpen(connection_hdl hdl, bool render)
{
    std::cout << "socket opened!" << std::endl;

	if (render) {
		mRenderHdl = hdl;
		uint16_t first_message[4];
		memcpy(first_message, "jsmp", 4);
		first_message[2] = htons(300);
		first_message[3] = htons(300);
		mRenderServer.send(hdl, first_message, 8, websocketpp::frame::opcode::binary);
	} else {
		{
			std::lock_guard<std::mutex> lk(mInputConMtx);
			mInputHdl = hdl;
		}
		mInputConCv.notify_one();
	}
	++mConnections;
}

void ServerApp::_onClose(connection_hdl hdl)
{
    std::cout << "socket closed!" << std::endl;
	--mConnections;
}