#include "OptionsMenu.h"
#include "SoundManager.h"

using namespace Ogre;

OptionsMenu::OptionsMenu()
{
    m_bQuit         = false;
    m_FrameEvent    = Ogre::FrameEvent();
}

void OptionsMenu::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering OptionsMenu...");

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "OptionsSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pCamera = m_pSceneMgr->createCamera("MenuCam");
    m_pCamera->setPosition(Vector3(0, 25, -50));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_CENTER, "OptionsLbl", "Options", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "SoundBtn", "Sound", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackBtn", "Back", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "OptionsMenuLbl", "Options mode", 250);

	m_bQuit = false;

    createScene();
}

void OptionsMenu::createScene()
{
}

void OptionsMenu::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving OptionsMenu...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);
}

bool OptionsMenu::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

bool OptionsMenu::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

bool OptionsMenu::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
    return true;
}

bool OptionsMenu::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;
    return true;
}

bool OptionsMenu::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;
    return true;
}

void OptionsMenu::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        popAppState();
        return;
    }
}

void OptionsMenu::buttonHit(OgreBites::Button *button)
{
  SoundManager::GetInstance()->PlaySoundOnce("MenuButton.wav");
    if(button->getName() == "BackBtn")
		m_bQuit = true;
    else if (button->getName() == "SoundBtn")
        changeAppState(findByName("SoundState"));
}
