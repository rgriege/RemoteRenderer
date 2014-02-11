#include "CharacterCreateState.h"
#include <sstream>
#include <cctype>
#include <pugixml.hpp>
#include <StringHelper.h>

using namespace Ogre;

void CharacterCreateState::enter()
{
  CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"CharacterCreate.layout"); 

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering CharacterCreateState...");
	
	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "CharCreateSceneMgr");
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

  pugi::xml_parse_result result = doc.load_file("xml/database.xml");
  std::cout << "XML Load result: " << result.description() << std::endl;
  root = doc.child("database");

	m_bQuit = false;
	buildGUI();
	createScene();
}

bool CharacterCreateState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing CharacterCreateState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);

	return true;
}

void CharacterCreateState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming CharacterCreateState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("CHAR_SEL_GUI"));

	m_bQuit = false;
}

void CharacterCreateState::createScene()
{
  m_pSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  m_pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}

void CharacterCreateState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving CharacterCreateState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
  CEGUI::WindowManager::getSingleton().destroyWindow("CHAR_CREATE_GUI");
	
	m_pSceneMgr->destroyCamera(m_pCamera);
	if(m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

bool CharacterCreateState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectKeyDown(keyEventRef.key);
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectChar(keyEventRef.text);

	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	return true;
}

bool CharacterCreateState::keyReleased(const OIS::KeyEvent &keyEventRef)
{		
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

bool CharacterCreateState::mouseMoved(const OIS::MouseEvent &evt)
{
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseWheelChange(evt.state.Z.rel);
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	return true;
}

bool CharacterCreateState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);

	return true;
}

bool CharacterCreateState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);

	return true;
}

bool CharacterCreateState::onCreateCharacterButton(const CEGUI::EventArgs &args)
{
  CEGUI::WindowManager::getSingleton().getWindow("SpaceError/Text")->setVisible(false);
  CEGUI::WindowManager::getSingleton().getWindow("CharacterExists/Text")->setVisible(false);
  CEGUI::WindowManager::getSingleton().getWindow("LengthError/Text")->setVisible(false);
  CEGUI::Editbox* charNameBox = (CEGUI::Editbox*)m_pMainWnd->getChild("CharacterName");
  std::string charactername = charNameBox->getText().c_str();

  if(charactername.find_first_of(' ') != std::string::npos)
  {
    CEGUI::WindowManager::getSingleton().getWindow("SpaceError/Text")->setVisible(true);
  } else if (charactername.length() > 15) {
  	CEGUI::WindowManager::getSingleton().getWindow("LengthError/Text")->setVisible(true);
  } else {
    pugi::xml_node characterNode;
    for( pugi::xml_node node = root.child("player"); node; node = node.next_sibling("player")) {
      pugi::xml_node result = node.find_child_by_attribute("character", "charactername", charNameBox->getText().c_str());
        if (result)
          characterNode = result;
    }
    if (characterNode) {
      printf("CreateCharacter: Found Character\n");
      CEGUI::WindowManager::getSingleton().getWindow("CharacterExists/Text")->setVisible(true);
    } else {
      pugi::xml_node charNode = root.append_child("character");
      charNode.append_attribute("charactername").set_value(charNameBox->getText().c_str());
      charNode.append_attribute("level").set_value(1);
      charNode.append_attribute("health").set_value(200);
      charNode.append_attribute("experience").set_value(0);
      charNode.append_attribute("damage").set_value(10);
      charNode.append_child("inventory").append_attribute("gold").set_value(0);
      charNode.append_child("equip");

      printf("Saving Character to File: xml/database.xml\n");
      doc.save_file("xml/database.xml");
      changeAppState(findByName("CharacterSelectState")); 
    }
  }
  return true;
}

void CharacterCreateState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
	{
		popAllAndPushAppState(findByName("MenuState"));
		return;
	}
}

void CharacterCreateState::buildGUI()
{
  CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();
  m_pMainWnd = windMgr.getWindow("CHAR_CREATE_GUI");

  CEGUI::PushButton* createCharBtn = (CEGUI::PushButton*)m_pMainWnd->getChild("CreateChar");
  createCharBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharacterCreateState::onCreateCharacterButton, this));
    
  OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);
}
