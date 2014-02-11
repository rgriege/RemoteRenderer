#include "AdvancedOgreFramework.h"

using namespace Ogre;

#ifdef _WIN32
template<> OgreFramework* Ogre::Singleton<OgreFramework>::msSingleton = 0;
#else
template<> OgreFramework* Ogre::Singleton<OgreFramework>::ms_Singleton = 0;
#endif

OgreFramework::OgreFramework()
{
    m_pRoot				  = 0;
    m_pRenderWnd		= 0;
    m_pViewport			= 0;
    m_pLog				  = 0;
    m_pTimer			  = 0;
	  mResourcesCfg		= Ogre::StringUtil::BLANK;
	  mPluginsCfg			= Ogre::StringUtil::BLANK;

    m_pInputMgr			= 0;
    m_pKeyboard			= 0;
    m_pMouse			  = 0;
    m_pTrayMgr      = 0;
}

OgreFramework::~OgreFramework()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
    if(m_pTrayMgr)      delete m_pTrayMgr;
    if(m_pInputMgr)		OIS::InputManager::destroyInputSystem(m_pInputMgr);
    if(m_pRoot)			delete m_pRoot;
}

bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{

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

    if(!m_pRoot->restoreConfig() && !m_pRoot->showConfigDialog())
        return false;
    m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

    m_pViewport = m_pRenderWnd->addViewport(0);
    m_pViewport->setBackgroundColour(ColourValue(0.5f, 0.5f, 0.5f, 1.0f));

    m_pViewport->setCamera(0);

    size_t hWnd = 0;
    OIS::ParamList paramList;
    m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

    m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
    m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();

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



    return true;
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
