#include "RemoteOgreFramework.h"
#include <RemoteOISWebSocketppConnection.h>

template<> OgreFramework* Ogre::Singleton<OgreFramework>::msSingleton = 0;

OgreFramework::OgreFramework(char* remoteHost, char* remotePort, char* localPort) :
    m_pRoot(0),
    m_pRenderWnd(0),
    m_pViewport(0),
    m_pLog(0),
    m_pTimer(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    m_pInputMgr(0),
    m_pKeyboard(0),
    m_pMouse(0),
    mRemoteHost(remoteHost),
    mRemotePort(atoi(remotePort)),
    mLocalPort(atoi(localPort))
{
    mShutdown = false;
}

OgreFramework::~OgreFramework()
{
    m_pLog->logMessage("Shutdown OGRE...");

    /* Stop server */
    mRenderServer.stop();
    mInputServer.stop();
    mRenderThread->join();
    mInputThread->join();
    delete mRenderThread;
    delete mInputThread;

    if(mEncoder)       delete mEncoder;
    if(m_pTrayMgr)      delete m_pTrayMgr;
    if(m_pInputMgr)     OIS::InputManager::destroyInputSystem(m_pInputMgr);
    if(m_pRoot)         delete m_pRoot;
}

bool OgreFramework::initOgre(Ogre::String wndTitle,
                                   OIS::KeyListener *pKeyListener,
                                   OIS::MouseListener *pMouseListener,
                                   Location renderLocation,
                                   Location inputLocation)
{
    mConnections = 0;
    mRenderServer.init_asio();
    mRenderServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
    mRenderServer.clear_access_channels(websocketpp::log::alevel::frame_header);
    mRenderServer.set_open_handler(bind(&OgreFramework::_onRenderOpen, this, _1));
    mRenderServer.set_close_handler(bind(&OgreFramework::_onClose, this, _1));
    mRenderServer.listen(mLocalPort);
    mRenderServer.start_accept();

    mInputServer.init_asio();
    mInputServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
    mInputServer.clear_access_channels(websocketpp::log::alevel::frame_header);
    mInputServer.set_open_handler(bind(&OgreFramework::_onInputOpen, this, _1));
    mInputServer.set_close_handler(bind(&OgreFramework::_onClose, this, _1));
    mInputServer.listen(mLocalPort+1);
    mInputServer.start_accept();
    mRenderThread = new std::thread(std::bind(&server::run, &mRenderServer));
    mInputThread = new std::thread(std::bind(&server::run, &mInputServer));

#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    Ogre::LogManager* logMgr = new Ogre::LogManager();

    m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
    m_pLog->setDebugOutputEnabled(true);

    m_pRoot = new Ogre::Root(mPluginsCfg);

    if(!m_pRoot->showConfigDialog())
        return false;
    m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

    m_pViewport = m_pRenderWnd->addViewport(0);
    m_pViewport->setBackgroundColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

    m_pViewport->setCamera(0);

#pragma push_macro("PixelFormat")
#undef PixelFormat
    mBuffer = Ogre::PixelBox(m_pRenderWnd->getWidth(), m_pRenderWnd->getHeight(), 1,
                            Ogre::PF_B8G8R8, malloc(m_pRenderWnd->getWidth()*m_pRenderWnd->getHeight()*3));
#pragma pop_macro("PixelFormat")

    if (inputLocation == REMOTE) {
        std::unique_lock<std::mutex> lk(mInputConMtx);
        mInputConCv.wait(lk, [&]() { return !mInputHdl._empty(); });
        lk.unlock();
        RemoteOIS::Connection* con = new RemoteOIS::WebSocketppConnection(mInputServer.get_con_from_hdl(mInputHdl));
        m_pInputMgr = RemoteOIS::InputManager::createInputSystem(con);
    } else {
        size_t hWnd = 0;
        OIS::ParamList paramList;
        m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);
        paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));
        m_pInputMgr = OIS::InputManager::createInputSystem(paramList);
    }

    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

    m_pMouse->getMouseState().height   = m_pRenderWnd->getHeight();
    m_pMouse->getMouseState().width    = m_pRenderWnd->getWidth();

    if(pKeyListener == 0)
        m_pKeyboard->setEventCallback(this);
    else
        m_pKeyboard->setEventCallback(pKeyListener);

    if(pMouseListener == 0)
        m_pMouse->setEventCallback(this);
    else
        m_pMouse->setEventCallback(pMouseListener);

    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;

    cf.load(mResourcesCfg);

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //m_pTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", m_pRenderWnd, m_pMouse, 0);
    m_pGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem(*m_pRenderWnd);
    m_pGUISystem = CEGUI::System::getSingletonPtr();
    CEGUI::SchemeManager::getSingleton().create((CEGUI::utf8*)"TaharezLook.scheme"); 
    CEGUI::ImagesetManager::getSingleton().create( "SkillSlot.imageset" ); 
    CEGUI::ImagesetManager::getSingleton().create( "Inventory.imageset" );
    CEGUI::ImagesetManager::getSingleton().create( "MiscImages.imageset" );

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();

    m_pRenderWnd->setActive(true);
    m_pRoot->addFrameListener(this);
    
    Ogre::WindowEventUtilities::addWindowEventListener(static_cast<Ogre::RenderWindow*>(m_pRenderWnd), this);

    return true;
}

bool OgreFramework::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    m_pRenderWnd->copyContentsToMemory(mBuffer, Ogre::RenderTarget::FB_AUTO);

    // create the packet
    AVPacket* pkt = new AVPacket();
    av_init_packet(pkt);
    mEncoder->write_rgb_data_to_frame(static_cast<uint8_t*>(mBuffer.data));
    mEncoder->encode_frame(*pkt);
    try {
        mRenderServer.send(mRenderHdl, pkt->data, pkt->size, websocketpp::frame::opcode::binary);
    }
    catch (websocketpp::lib::error_code& e) {
        std::cout << "Frame send failed: " << e.message() << std::endl;
    }
    av_free_packet(pkt);

    return true;
}

void OgreFramework::windowResized(Ogre::RenderWindow* rw)
{
     
}

void OgreFramework::windowClosed(Ogre::RenderWindow* rw)
{
    mShutdown = true;
}

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
    {
        m_pRenderWnd->writeContentsToTimestampedFile("AOF_Screenshot_", ".jpg");
        return true;
    }

    return true;
}

bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return true;
}

OIS::MouseState OgreFramework::scaleMouseState(const OIS::MouseState& state)
{
    OIS::MouseState result;
    result.X.abs = (state.X.abs * m_pRenderWnd->getWidth()) / state.width;
    result.X.rel = (state.X.rel * m_pRenderWnd->getWidth()) / state.width;
    result.Y.abs = (state.Y.abs * m_pRenderWnd->getHeight()) / state.height;
    result.Y.rel = (state.Y.rel * m_pRenderWnd->getHeight()) / state.height;
    result.width = m_pRenderWnd->getWidth();
    result.height = m_pRenderWnd->getHeight();
    result.buttons = state.buttons;
    return result;
}

bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
    OIS::MouseState state = scaleMouseState(evt.state);
    return true;
}

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    OIS::MouseState state = scaleMouseState(evt.state);
    return true;
}

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    OIS::MouseState state = scaleMouseState(evt.state);
    return true;
}

void OgreFramework::updateOgre(double timeSinceLastFrame)
{
}

bool OgreFramework::closed()
{
    return mShutdown;
}

void OgreFramework::_onRenderOpen(connection_hdl hdl)
{
    std::cout << "render socket opened!" << std::endl;

    server::connection_ptr con = mRenderServer.get_con_from_hdl(hdl);
    if (con->get_host() != mRemoteHost && con->get_port() != mRemotePort) {
        std::cout << "Unidentified client" << std::endl;
        mRenderServer.close(hdl, websocketpp::close::status::policy_violation, "Invalid IP");
        return;
    }
    mRenderHdl = hdl;
    uint16_t first_message[4];
    memcpy(first_message, "jsmp", 4);
    first_message[2] = htons(300);
    first_message[3] = htons(300);
    mRenderServer.send(hdl, first_message, 8, websocketpp::frame::opcode::binary);

    ++mConnections;
}

void OgreFramework::_onInputOpen(connection_hdl hdl)
{
    std::cout << "input socket opened!" << std::endl; {
        std::lock_guard<std::mutex> lk(mInputConMtx);
        mInputHdl = hdl;
    }
    mInputConCv.notify_one();
    ++mConnections;
}

void OgreFramework::_onClose(connection_hdl hdl)
{
    std::cout << "socket closed!" << std::endl;
    --mConnections;
    if (mConnections == 0) {
        std::cout << "shutdown initiated" << std::endl;
        mShutdown = true;
    }
}