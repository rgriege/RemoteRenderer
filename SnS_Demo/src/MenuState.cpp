#include "MenuState.h"

using namespace Ogre;

void MenuState::enter()
{
  CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"MainMenu.layout"); 

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering MenuState...");
	
	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "MenuSceneMgr");
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7)); 

	m_pCamera = m_pSceneMgr->createCamera("MenuCam");
	m_pCamera->setPosition(Vector3(0, 0, -20));
	m_pCamera->lookAt(Vector3(0, 0, 0));
	m_pCamera->setNearClipDistance(1);

	m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) / 
							  Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));
	
	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pKeyboard->setEventCallback(this);
	OgreFramework::getSingletonPtr()->m_pMouse->setEventCallback(this);


	m_bQuit = false;
	buildGUI();
	createScene();
}

bool MenuState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing MenuState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);

	return true;
}

void MenuState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming MenuState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI"));

	m_bQuit = false;
}

void MenuState::createScene()
{
  m_pSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  m_pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}

void MenuState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving MenuState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
  CEGUI::WindowManager::getSingleton().destroyWindow("AOF_GUI");
	
	m_pSceneMgr->destroyCamera(m_pCamera);
	if(m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

bool MenuState::onExitButton(const CEGUI::EventArgs &args)
{
	m_bQuit = true;
	return true;
}

bool MenuState::onEnterButton(const CEGUI::EventArgs &args)
{
	changeAppState(findByName("CharacterSelectState"));
	return true;
}

bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	return true;
}

bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{		
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

bool MenuState::mouseMoved(const OIS::MouseEvent &evt)
{
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseWheelChange(evt.state.Z.rel);
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	return true;
}

bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);

	return true;
}

bool MenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);

	return true;
}

void MenuState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		this->popAppState();
		return;
	}
}

void MenuState::buildGUI()
{
	OgreFramework::getSingletonPtr()->m_pGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
  CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
	const OIS::MouseState state = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState();
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
	CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

	CEGUI::Window* pMainWnd = CEGUI::WindowManager::getSingleton().getWindow("AOF_GUI");
  OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(pMainWnd);

	CEGUI::PushButton* button = (CEGUI::PushButton*)pMainWnd->getChild("ExitButton");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onExitButton, this));
	button = (CEGUI::PushButton*)pMainWnd->getChild("EnterButton");
	button->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onEnterButton, this));
}
