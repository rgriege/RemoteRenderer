#include "SoundMenu.h"
#include "SoundManager.h"

using namespace Ogre;

SoundMenu::SoundMenu()
{
    m_bQuit         = false;
    m_FrameEvent    = Ogre::FrameEvent();
}

void SoundMenu::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering SoundMenu...");

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "SoundSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pCamera = m_pSceneMgr->createCamera("SoundCam");
    m_pCamera->setPosition(Vector3(0, 25, -50));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();
	  OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_CENTER, "SoundLbl", "Sound Options", 310);
    OgreBites::Slider *mslider = OgreFramework::getSingletonPtr()->m_pTrayMgr->
      createLongSlider(OgreBites::TL_CENTER, "MusSlider", "Music Volume ", 130, 55, 0, MIX_MAX_VOLUME, 129);
    OgreBites::Slider *sslider = OgreFramework::getSingletonPtr()->m_pTrayMgr->
      createLongSlider(OgreBites::TL_CENTER, "SndSlider", "Sound Volume", 130, 55, 0, MIX_MAX_VOLUME, 129);
    mslider->setValue(SoundManager::GetInstance()->GetMusicVolume());
    sslider->setValue(SoundManager::GetInstance()->GetSoundVolume());
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackBtn", "Back", 310);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "SoundMenuLbl", "Sound mode", 250);

    createScene();
}

void SoundMenu::createScene()
{
}

void SoundMenu::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving SoundMenu...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);
}

bool SoundMenu::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

bool SoundMenu::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

bool SoundMenu::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
    return true;
}

bool SoundMenu::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;
    return true;
}

bool SoundMenu::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;
    return true;
}

void SoundMenu::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        shutdown();
        return;
    }
}

void SoundMenu::buttonHit(OgreBites::Button *button)
{
  SoundManager::GetInstance()->PlaySoundOnce("GameOver.wav");
    if(button->getName() == "BackBtn")
        changeAppState(findByName("OptionsState"));
}

void SoundMenu::sliderMoved(OgreBites::Slider *slider)
{
  Ogre::Real value = slider->getValue();
  if(slider->getName() == "MusSlider")
    SoundManager::GetInstance()->SetMusicVolume(value);
  else if(slider->getName() == "SndSlider")
    SoundManager::GetInstance()->SetSoundVolume(value);
}
