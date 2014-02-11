#include "CharacterSelectState.h"
#include "GameState.h"
#include "pugixml.hpp"
#include "StringHelper.h"
#include <sstream>

using namespace Ogre;

void CharacterSelectState::enter()
{
  CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"CharacterSelect.layout"); 

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering CharacterSelectState...");
	
	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "CharSceneMgr");
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
	printf("done\n");
}

bool CharacterSelectState::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing CharacterSelectState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);

	return true;
}

void CharacterSelectState::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming CharacterSelectState...");

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(CEGUI::WindowManager::getSingleton().getWindow("CHAR_SEL_GUI"));

	m_bQuit = false;
}

void CharacterSelectState::createScene()
{
  m_pSceneMgr->setAmbientLight(Ogre::ColourValue(1, 1, 1));
  m_pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}

void CharacterSelectState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving CharacterSelectState...");

	OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(0);
  CEGUI::WindowManager::getSingleton().destroyWindow("CHAR_SEL_GUI");
	
	m_pSceneMgr->destroyCamera(m_pCamera);
	if(m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

bool CharacterSelectState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	return true;
}

bool CharacterSelectState::keyReleased(const OIS::KeyEvent &keyEventRef)
{		
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);

	return true;
}

bool CharacterSelectState::mouseMoved(const OIS::MouseEvent &evt)
{
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseWheelChange(evt.state.Z.rel);
	OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

	return true;
}

bool CharacterSelectState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);

	return true;
}

bool CharacterSelectState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);

	return true;
}

void CharacterSelectState::update(double timeSinceLastFrame)
{
	if(m_bQuit == true)
		popAllAndPushAppState(findByName("MenuState"));
}

bool CharacterSelectState::onCharacterSelButton(const CEGUI::EventArgs &args)
{
  const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(args);
  character = we.window->getName();
  CEGUI::Window* w = CEGUI::WindowManager::getSingleton().getWindow("Character");
  w->setText(character);

  return true;
}

bool CharacterSelectState::onCharacterCreateButton(const CEGUI::EventArgs &args)
{
  changeAppState(findByName("CharacterCreateState"));
  return true;
}

bool CharacterSelectState::onEnterWorldButton(const CEGUI::EventArgs &args)
{
  if (!character.empty()) {
    GameState::playerData = root.find_child_by_attribute("character", "charactername", character.c_str());
    changeAppState(findByName("GameState"));
  }
  return true;
}

void CharacterSelectState::CreateCharacterButtons(std::string data)
{
	if (data.compare("EMPTY") != 0 ) {
	  std::vector<std::string> characters = StringHelper::split(data, ';');
	  std::cout << "character data: " << data << std::endl;
	  charactersData = data;
	  
	  for(int i = 0; i < characters.size(); i++) {
	  	std::vector<std::string> charData = StringHelper::split(characters[i], ' ');
	    CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();

	    CEGUI::PushButton *tempBtn = (CEGUI::PushButton*)windMgr.createWindow("TaharezLook/Button", charData[0]);
	    tempBtn->setHorizontalAlignment(CEGUI::HA_CENTRE);
	    tempBtn->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.0f, 0.0f), CEGUI::UDim( 0.015f+(i*0.11), 0.0f ) ) );
	    tempBtn->setSize( CEGUI::UVector2( CEGUI::UDim( 0.9f, 0.0f ), CEGUI::UDim( 0.10f, 0.0f ) ) );
	    std::stringstream ss;
	    ss << charData[0] << "\n" << "Level " << charData[1];
	    tempBtn->setText(ss.str());
	    tempBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharacterSelectState::onCharacterSelButton, this));
	    m_pCharWnd->addChildWindow(tempBtn);
	  }
	  printf("%i Character buttons created\n", characters.size());
	}
}

void CharacterSelectState::buildGUI()
{
  CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();

  m_pMainWnd = windMgr.getWindow("CHAR_SEL_GUI");

  m_pCharWnd = windMgr.getWindow("CharSelectWnd");
  CEGUI::PushButton* createChar = (CEGUI::PushButton*)m_pCharWnd->getChild("CreateChar");
  createChar->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharacterSelectState::onCharacterCreateButton, this));

  CEGUI::PushButton* enterWorld = (CEGUI::PushButton*)m_pMainWnd->getChild("EnterWorld");
  enterWorld->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&CharacterSelectState::onEnterWorldButton, this));

  ParseDatabase();
  stringstream ss; 
  for( pugi::xml_node node = root.child("character"); ; ) {
    if (node) {
      for (pugi::xml_attribute_iterator ait = node.attributes_begin(); ; ) {
        ss << ait->value();
        ++ait;
        if(ait != node.attributes_end())
          ss << " ";
        else
          break;
      }
      node = node.next_sibling("character");
      if (node)
        ss << ";";
      else
        break;
    } else {
      ss << "EMPTY";
      break;
    }
  }
  CreateCharacterButtons(ss.str());

  OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);
}

void CharacterSelectState::ParseDatabase()
{
  pugi::xml_parse_result result = doc.load_file("xml/database.xml");
  std::cout << "XML Load result: " << result.description() << std::endl;
  root = doc.child("database");
}