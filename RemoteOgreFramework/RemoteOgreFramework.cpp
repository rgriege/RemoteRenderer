#include "RemoteOgreFramework.h"
#include <RemoteOISWebSocketppConnection.h>

template<> OgreFramework* Ogre::Singleton<OgreFramework>::msSingleton = 0;

OgreFramework::OgreFramework(std::string remoteHost, uint16_t remotePort, uint16_t localPort) :
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
    mEncoder(0),
    m_pTrayMgr(0),
    mRemoteHost(remoteHost),
    mRemotePort(remotePort),
    mLocalPort(localPort),
    mInputLocation(LOCAL),
    mRenderLocation(LOCAL)
{
    mShutdown = false;
    mBuffer.data = 0;
    mConnections = 0;
}

OgreFramework::~OgreFramework()
{
    if (m_pLog)
        m_pLog->logMessage("Shutdown OGRE...");

    /* Stop server */
    if (mInputLocation == REMOTE) {
        mInputServer.stop();
        mInputThread->join();
        delete mInputThread;
    }
    if (mRenderLocation == REMOTE) {
        mRenderServer.stop();
        mRenderThread->join();
        delete mRenderThread;
    }

    if(mEncoder)        delete mEncoder;
    if(mBuffer.data)    free(mBuffer.data);
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
    mInputLocation = inputLocation;
    mRenderLocation = renderLocation;

    if ((inputLocation == REMOTE || renderLocation == REMOTE) &&
        (mLocalPort == 0 || mRemoteHost == "" || mRemotePort == 0))
        return false;
    if (renderLocation == REMOTE) {
        mRenderServer.init_asio();
        mRenderServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
        mRenderServer.clear_access_channels(websocketpp::log::alevel::frame_header);
        mRenderServer.set_open_handler(bind(&OgreFramework::_onRenderOpen, this, _1));
        mRenderServer.set_close_handler(bind(&OgreFramework::_onClose, this, _1));
        mRenderServer.listen(mLocalPort);
        mRenderServer.start_accept();
        mRenderThread = new std::thread(std::bind(&server::run, &mRenderServer));
    }
    if (inputLocation == REMOTE) {
        mInputServer.init_asio();
        mInputServer.clear_access_channels(websocketpp::log::alevel::frame_payload);
        mInputServer.clear_access_channels(websocketpp::log::alevel::frame_header);
        mInputServer.set_open_handler(bind(&OgreFramework::_onInputOpen, this, _1));
        mInputServer.set_close_handler(bind(&OgreFramework::_onClose, this, _1));
        mInputServer.listen(mLocalPort+1);
        mInputServer.start_accept();
        mInputThread = new std::thread(std::bind(&server::run, &mInputServer));
    }

    Ogre::LogManager* logMgr = new Ogre::LogManager();
    m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
    m_pLog->setDebugOutputEnabled(true);

#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    m_pRoot = new Ogre::Root(mPluginsCfg);
    if (mRenderLocation == REMOTE && !m_pRoot->restoreConfig())
        return false;
    if (mRenderLocation == LOCAL && !m_pRoot->showConfigDialog())
        return false;
    {
        std::lock_guard<std::mutex> lk(mRenderConMtx);
        m_pRenderWnd = m_pRoot->initialise(true, wndTitle);
    }

    m_pViewport = m_pRenderWnd->addViewport(0);
    m_pViewport->setBackgroundColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f));
    m_pViewport->setCamera(0);

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

    // the SdkTrayManager only needs a RenderTarget but asks for a RenderWindow
    m_pTrayMgr = new OgreBites::SdkTrayManager("AOFTrayMgr", static_cast<Ogre::RenderWindow*>(m_pRenderWnd), m_pMouse, 0);

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();

    if (mRenderLocation == REMOTE) {
        /*m_pRenderWnd = Ogre::TextureManager::getSingleton().createManual(
            "RttTex", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D,
            300, 300, 0, Ogre::PF_R8G8B8, Ogre::TU_RENDERTARGET)->getBuffer()->getRenderTarget();
        m_pViewport = renderTex->addViewport(m_pCamera);*/
#pragma push_macro("PixelFormat")
#undef PixelFormat
        mBuffer = Ogre::PixelBox(m_pRenderWnd->getWidth(), m_pRenderWnd->getHeight(), 1,
                                Ogre::PF_B8G8R8, malloc(m_pRenderWnd->getWidth()*m_pRenderWnd->getHeight()*3));
#pragma pop_macro("PixelFormat")

        /* ffmpeg init, also try AV_CODEC_ID_H264 */
        mEncoder = new Encoder();
        if (!mEncoder->bootstrap(AV_CODEC_ID_MPEG1VIDEO, m_pRenderWnd->getWidth(), m_pRenderWnd->getHeight(), 30))
            return false;

        m_pRoot->addFrameListener(this);
        // Can only be added once RenderSystem_RemoteGL is added to Ogre
        // Ogre::WindowEventUtilities::addWindowEventListener(static_cast<Ogre::RenderWindow*>(m_pRenderWnd), this);
    }

    m_pRenderWnd->setActive(true);

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
    m_pMouse->getMouseState().width = rw->getWidth();
    m_pMouse->getMouseState().height = rw->getHeight();
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

bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
    return true;
}

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    return true;
}

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
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
    std::unique_lock<std::mutex> lk(mRenderConMtx);
    mRenderConCv.wait(lk, [&]() { return m_pRenderWnd; });
    lk.unlock();
    first_message[2] = htons(m_pRenderWnd->getWidth());
    first_message[3] = htons(m_pRenderWnd->getHeight());
    mRenderServer.send(hdl, first_message, 8, websocketpp::frame::opcode::binary);

    ++mConnections;
}

void OgreFramework::_onInputOpen(connection_hdl hdl)
{
    std::cout << "input socket opened!" << std::endl;
    {
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