#include "GameState.h"
#include "btBulletDynamicsCommon.h"
#include "World.h"
#include "SoundManager.h"
#include "Item.h"
#include <typeinfo>
#include <sstream>


bool GameState::m_bPause = false;
pugi::xml_node GameState::playerData;

GameState::GameState()
{
  m_bQuit             = false;
	m_bSettingsMode		= false;
	m_bPan				= false;
	mMove = 0.08f;
  mRotate = 0.002f;
	mZoom = 0.008f;
	mDirection = Ogre::Vector3::ZERO;
  menuToggle = false;
  inventoryToggle = false;
  characterToggle = false;
  deathCoilCoolDown = false;
  deathCoilCounter = 2000;
  deathCoilRange = 20;
  deathCoilCost = 50;
  healCoolDown = false;
  healCounter = 500;
  healRange = 15;
  healCost = 25;
  manaRegen = 5;
  manaRegenFlag = false;
  manaRegenCounter = 2000;
  timeSinceLastInput = 0;
}

void GameState::enter()
{
    CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"Game.layout"); 
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.f, 0.f, 0.f));

    createScene();
    buildGUI();

    SoundManager::GetInstance()->SetMusicVolume(16);
    SoundManager::GetInstance()->PlayBgm("countryside.wav");
}

bool GameState::pause()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");

    return true;
}

void GameState::resume()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");

    buildGUI();

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_bQuit = false;
}
void GameState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");

    CEGUI::WindowManager::getSingleton().destroyWindow("GAME_GUI");
    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    //NetworkManager::GetInstance()->Close();
}

void GameState::createScene()
{
  //Set up RaySceneQuery
  mRayScnQuery = m_pSceneMgr->createRayQuery(Ogre::Ray());
	//Set up Camera
	m_pCamera = m_pSceneMgr->createCamera("GameCamera");

  m_pCamera->setPosition(Ogre::Vector3(50.f, 15.f, 0));
  m_pCamera->setNearClipDistance(5);
	m_pCamera->lookAt(50.f, 10.f, 10.f);
	mDirection = Ogre::Vector3(0, 0, 1.f);

  m_pCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
    Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

  OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
	// Set the scene's ambient light
	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

  printf("Creating world\n");
  world = new World(m_pSceneMgr);
  world->addPlayer(0, playerData.attribute("charactername").as_string());
  player = world->getPlayer(0);
  world->setPlayer(playerData, 0);

  // Set the camera
  Ogre::Vector3 playerPos = player->getPosition();
  playerPos.y = player->getHeight()/2.f;
  Ogre::Radian playerYaw = player->getYaw();
  m_pCamera->setPosition(playerPos);
  m_pCamera->yaw(playerYaw);
  m_pCamera->moveRelative(Ogre::Vector3(0, 20.f, 80.f));

  // Create a point light
  Ogre::Light* plight = m_pSceneMgr->createLight("PointLight");
  plight->setType(Ogre::Light::LT_POINT);
	plight->setSpecularColour(1, 1, 1);
	plight->setDiffuseColour(1, 1, 1);
	plight->setPosition(0, 2000, 0);

  printf("scene created\n");
}

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
      toggleGameMenu();
    }
    else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_I)) {
      toggleInventory();
    }
    else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_C)) {
      toggleCharacterSheet();
    }

    return true;
}

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{

  if(keyEventRef.key == OIS::KC_1) {
    //std::stringstream attackStream;
    //attackStream << "P1" << player->isAttacking();
    ToggleAttack();
  }
  if(keyEventRef.key == OIS::KC_2 && player->getTarget() != NULL && player->getTarget()->getHealth() > 0 && !deathCoilCoolDown
&& floor((player->getPosition() - player->getTarget()->getPosition()).length()) * 0.02f < deathCoilRange && player->getMana() >= deathCoilCost
&& typeid(*player->getTarget()) == typeid(Enemy)) {

    player->setSpellAttacking(true);
    world->addProjectile(DEATHCOIL, player); 

    deathCoilCoolDown = true;
    player->setMana(player->getMana() - deathCoilCost);
    m_pMainWnd->getChild("HUD/Skill2")->setVisible(false);
    m_pMainWnd->getChild("HUD/Skill2Cooldown")->setVisible(true);
  }
  if(keyEventRef.key == OIS::KC_3 && player->getTarget() != NULL && !healCoolDown && typeid(*player->getTarget()) == typeid(Player)
&& floor((player->getPosition() - player->getTarget()->getPosition()).length()) * 0.02f < healRange && player->getMana() >= healCost) {

    player->setSpellAttacking(true);
    Character* targetPlayer = player->getTarget();
    if(targetPlayer->getHealth() <= targetPlayer->getMaxHealth() - 50)
    	targetPlayer->setHealth(targetPlayer->getHealth() + 50);
    else
	  targetPlayer->setHealth(targetPlayer->getMaxHealth());

    healCoolDown = true;
    player->setMana(player->getMana() - healCost);
    m_pMainWnd->getChild("HUD/Skill3")->setVisible(false);
    m_pMainWnd->getChild("HUD/Skill3Cooldown")->setVisible(true);
  }
  else
    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
	
  return true;
}

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
	// either + or - 120
	if(evt.state.Z.rel != 0) {
		Ogre::Degree new_fovy = Ogre::Degree(m_pCamera->getFOVy())-Ogre::Degree(mZoom*evt.state.Z.rel);
		printf("%f\n", mZoom*evt.state.Z.rel);
		printf("%f\n", m_pCamera->getFOVy().valueDegrees());
		//printf("%f\n", new_fovy);
		if (new_fovy > Ogre::Degree(40.f) && new_fovy < Ogre::Degree(60.f))
			m_pCamera->setFOVy(Ogre::Radian(new_fovy));
	}

	if (m_bPan && !menuToggle) {
    m_pCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.1));
    m_pCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.1));

    Ogre::Real pitchAngleSign = m_pCamera->getOrientation().x;
    Ogre::Real pitchAngle = Ogre::Math::Abs(m_pCamera->getOrientation().getPitch().valueDegrees());

    if (pitchAngle > 90.0f) {
      if (pitchAngleSign > 0) {
        m_pCamera->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f), 0, 0));
      }
      else if (pitchAngleSign < 0) {
        m_pCamera->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f), 0, 0)); 
      }
    }

  } else {
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
	}

    return true;
}

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
  printf("Handle Mouse Pressed\n");
  bool hasTarget = false;
	if (id == OIS::MB_Right) {
		m_bPan = true;
    CEGUI::MouseCursor::getSingleton().hide();
  }
  if(id == OIS::MB_Left){
    bool mouse_was_processed = OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonDown(CEGUI::LeftButton);
    
    if (!mouse_was_processed) {
      //If player selects something in the world
      printf("Mouse was not processed by CEGUI\n");
      CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
		  //Send Raycast
      Ogre::Ray mouseRay = m_pCamera->getCameraToViewportRay(mousePos.d_x/float(evt.state.width), mousePos.d_y/float(evt.state.height));
		  mRayScnQuery->setRay(mouseRay);
      
		  Ogre::RaySceneQueryResult& result = mRayScnQuery->execute();
		  Ogre::RaySceneQueryResult::iterator itr;

      if(m_pLootWnd->isVisible()) DestroyAndHideLootWindow();
      printf("Loot was not visible\n");

      for ( itr = result.begin( ); itr != result.end(); itr++ ) {
        if ( itr->worldFragment ) {
          Ogre::Vector3 location = itr->worldFragment->singleIntersection;
          std::cout << "WorldFragment: (" << location.x << ", " << location.y << ", " << location.z << ")" << std::endl;
        } 
        else if ( itr->movable ) {
          if (isNotIgnoreCase(itr->movable->getParentSceneNode()->getName())) {
            printf("Clicked on Enemy or Player\n");
            // first deselect the current target if one exists
            if (player->getTarget() != NULL)
              player->getTarget()->showBoundingBox(false);
            hasTarget = true;
            player->setTarget(world->getObjectByName(itr->movable->getParentSceneNode()->getName()));
            printf("Showing Bounding Box\n");
            player->getTarget()->showBoundingBox(true);
            //Show Target on HUD
            printf("Showing Target Window\n");
            m_pTargetWnd->setVisible(true);
            m_pTargetWnd->getChild("TargetWindow/TargetName")->setText(player->getTarget()->getName());
            if (player->getTarget()->isDead()) {
              CreateAndShowLootWindow(player->getTarget());
            }
          }
        }
      }
      if ( !hasTarget && player->getTarget() != NULL ) {
        printf("Removing Bounding Box\n");
        player->getTarget()->showBoundingBox(false);
        player->setTarget(NULL);
        //Remove Target from HUD and set to ""
        printf("Setting Target to false\n");
        m_pTargetWnd->setVisible(false);
        m_pTargetWnd->getChild("TargetWindow/TargetName")->setText("");
      }
      printf("Sending target to server\n");
      player->setTarget(player->getTarget());
    }
  }
  printf("Done Mouse Pressed\n");
  return true;
}

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (id == OIS::MB_Right) {
		m_bPan = false;
    CEGUI::MouseCursor::getSingleton().show();
  }
  if(id == OIS::MB_Left) {
		OgreFramework::getSingletonPtr()->m_pGUISystem->injectMouseButtonUp(CEGUI::LeftButton);
  }

  return true;
}

void GameState::update(double timeSinceLastFrame)
{
  if(m_bQuit == true) {
    printf("logging out from gamestate\n");
    shutdown();
    return;
  }
  mMoveScale = mMove * timeSinceLastFrame;
  mRotateScale = mRotate * timeSinceLastFrame;

  timeSinceLastInput = getAndSendKeyInput() ? 0 : timeSinceLastInput+1;
  bool rustling = SoundManager::GetInstance()->SoundPlaying("Rustling.wav");
  if (timeSinceLastInput == 0 && !rustling)
      SoundManager::GetInstance()->PlaySoundLoop("Rustling.wav");
  else if (timeSinceLastInput > 2 && rustling)
      SoundManager::GetInstance()->PauseSound("Rustling.wav");

  // adjust camera yaw
  Ogre::Vector3 cameraDirection = m_pCamera->getRealDirection();
  if(timeSinceLastInput == 0 && !m_bPan && cameraDirection != mDirection) {
    Ogre::Vector3 diff = mDirection - cameraDirection;
    if (diff.length() < 0.001f) {
      m_pCamera->setDirection(mDirection);
    } else {
      Ogre::Vector3 newDirection = cameraDirection + 0.05f*(mDirection - cameraDirection);
      m_pCamera->setDirection(newDirection);
    }
  }

    /* Step the world */
    world->step(timeSinceLastFrame);
    OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);

  // check that you didn't die
  if (player->getHealth() == 0) {
    printf("logging out from gamestate\n");
    shutdown();
    return;
  }

  mDirection = player->getDirection();

  // adjust camera position
  Ogre::Vector3 playerPos = player->getPosition();
  playerPos.y = player->getHeight()/2.f;
  Ogre::Radian playerYaw = player->getYaw();
  m_pCamera->setPosition(playerPos);
  m_pCamera->moveRelative(Ogre::Vector3(0, 20.f, 80.f));
  //printf("camera position set\n");

  // display fps
  std::stringstream ss;
  ss << "FPS: " << OgreFramework::getSingletonPtr()->m_pRenderWnd->getAverageFPS();
  m_pFpsWnd->setText(ss.str());

  // update healthbar
  CEGUI::ProgressBar* myHealth = (CEGUI::ProgressBar*)m_pMainWnd->getChild("HUD/HealthBar");
  myHealth->setProgress(player->getHealth()/(float)player->getMaxHealth());

  // update manabar
  if(manaRegenFlag && player->getMana() < player->getMaxMana())
  {
	  player->setMana(player->getMana() + manaRegen);
  	manaRegenFlag = false;
	  manaRegenCounter = 2000;
  }
  else if(!manaRegenFlag && manaRegenCounter > 0)
	  manaRegenCounter -= timeSinceLastFrame;
  else if(!manaRegenFlag && manaRegenCounter <= 0)
	  manaRegenFlag = true;
  CEGUI::ProgressBar* myMana = (CEGUI::ProgressBar*)m_pMainWnd->getChild("HUD/MagicBar");
  myMana->setProgress(player->getMana()/(float)player->getMaxMana());

  if (player->getTarget()) {
    ss.str(std::string());
    Ogre::Real distance = (player->getPosition() - player->getTarget()->getPosition()).length();
    ss << player->getTarget()->getName() << " " << floor(distance)*.02f << " m";
    m_pTargetWnd->getChild("TargetWindow/TargetName")->setText(ss.str());
    //std::cout << player->getTarget()->getName() << ": " << player->getTarget()->getHealth()<< "/" << player->getTarget()->getMaxHealth() << std::endl;

    CEGUI::ProgressBar* enemyHealth = (CEGUI::ProgressBar*)m_pTargetWnd->getChild("TargetWindow/HealthBar");
    enemyHealth->setProgress(player->getTarget()->getHealth()/(float)player->getTarget()->getMaxHealth());

    if ( player->getTarget()->isDead() && !player->getTarget()->hasGivenExp()) {
      player->getTarget()->hasAlreadyGivenExp();
      player->incrementExperience(player->getTarget()->getExp());
      CEGUI::ProgressBar* playerExp = (CEGUI::ProgressBar*)m_pMainWnd->getChild("HUD/ExperienceBar");
      playerExp->setProgress(player->getExp()/(float)player->getLevelExperienceMax());
    }
  }

  // damage effect updates
  Ogre::Matrix4 cameraTransform = m_pCamera->getProjectionMatrix() * m_pCamera->getViewMatrix();
  std::vector<Character*> chars = world->checkCharacterHealth();
  for (std::vector<Character*>::iterator it = chars.begin(); it != chars.end(); it++) {
    int num = rand() % 3 + 3;
    for (int i = 0; i < num; i++) {
      BoxChunk* c = new BoxChunk(m_pSceneMgr, *it);
      chunks.push_back(c);
    }
    Ogre::Vector3 point = cameraTransform * ((*it)->getPosition() + Ogre::Vector3(0, (*it)->getHeight(), 0));
    CEGUI::UVector2 screenPos(CEGUI::UDim((point.x / 2) + 0.f, 0),
                              CEGUI::UDim(-(point.y / 2) + 0.5f, -40));
    FloatingNumber* f = new FloatingNumber(m_pMainWnd, screenPos, (*it)->healthChange);
    fnums.push_back(f);
    (*it)->healthChange = 0;
  }
  if (!chars.empty())
    SoundManager::GetInstance()->PlaySoundOnce("Punch.wav");

  for(std::vector<FloatingNumber*>::iterator it = fnums.begin(); it != fnums.end(); ) {
    FloatingNumber* f = *it;
    f->update(timeSinceLastFrame);
    if (f->finished()) {
      it = fnums.erase(it);
      delete f;
    } else {
      it++;
    }
  }

  std::map<int, Player*> players = world->checkNewPlayers();
  for(std::map<int, Player*>::iterator it = players.begin(); it!= players.end(); ++it) {
    FloatingText* ft = new FloatingText(m_pMainWnd, it->second, it->second->getName());
    ftexts.push_back(ft);
  }
  for(std::vector<FloatingText*>::iterator it = ftexts.begin(); it != ftexts.end(); ++it) {
    FloatingText* ft = *it;
    Ogre::Vector3 diff = ft->getCharPosition() - m_pCamera->getPosition();
    Ogre::Real dot = m_pCamera->getDirection().dotProduct(diff);
    if (ft->isVisible() && dot < 0)
      ft->setVisible(false);
    if (!ft->isVisible() && dot > 0)
      ft->setVisible(true);
    (*it)->update(cameraTransform);
  }

  //Update inventory 
  clearInventoryButtons();
  for(int i = 0; i < player->getInventory().size(); i++) {
    Item* item = player->getInventory()[i];
    std::stringstream itemSlot, imageName;
    itemSlot << "Inventory/Slot" << (i+1);
    imageName << "set:InventorySlots image:" << item->getImage();
    SetButtonImage(m_pInventoryWnd->getChild(itemSlot.str()), imageName.str());
  }

  //Update Character Sheet
  clearEquipButtons();
  std::map<int, Item*> equipItems = player->getEquipItems();
  if (equipItems[WEAPON]){
    std::stringstream imageName;
    imageName << "set:InventorySlots image:" << equipItems[WEAPON]->getImage();
    SetButtonImage(m_pCharacterEquipWnd->getChild("EquipItems/Weapon"), imageName.str());
  }

  //if (world->checkPlayerGold(localPlayerNum))
    //SoundManager::GetInstance()->PlaySoundOnce("Coins.wav");

  if(deathCoilCoolDown && deathCoilCounter <= 0)
  {
  	deathCoilCounter = 2000;
  	deathCoilCoolDown = false;
    m_pMainWnd->getChild("HUD/Skill2")->setVisible(true);
    m_pMainWnd->getChild("HUD/Skill2Cooldown")->setVisible(false);
  }
  else if(deathCoilCoolDown && deathCoilCounter > 0)
	  deathCoilCounter -= timeSinceLastFrame;

  if(healCoolDown && healCounter <= 0)
  {
  	healCounter = 500;
  	healCoolDown = false;
    m_pMainWnd->getChild("HUD/Skill3")->setVisible(true);
    m_pMainWnd->getChild("HUD/Skill3Cooldown")->setVisible(false);
  }
  else if(healCoolDown && healCounter > 0)
	  healCounter -= timeSinceLastFrame;

  for(std::vector<BoxChunk*>::iterator it = chunks.begin(); it != chunks.end(); ) {
    BoxChunk* c = *it;
    c->update(timeSinceLastFrame);
    if (c->finished()) {
      it = chunks.erase(it);
      delete c;
    } else {
      it++;
    }
  }
}

void GameState::toggleGameMenu()
{
  menuToggle = !menuToggle;
  m_pGameMenuWnd->setVisible(menuToggle);
}

void GameState::toggleInventory()
{
  clearInventoryButtons();
  inventoryToggle = !inventoryToggle;
  m_pInventoryWnd->setVisible(inventoryToggle);
  if (m_pInventoryWnd->isVisible()) {
    std::stringstream ss;
    ss << "Inventory (" << player->getGold() << "g)";
    m_pInventoryWnd->setText(ss.str());
  }
  std::cout << player->getInventory().size() << " items" << std::endl;

}

void GameState::toggleCharacterSheet()
{
  characterToggle = !characterToggle;
  m_pCharacterWnd->setVisible(characterToggle);
  std::stringstream healthStream, damageStream, expStream, levelStream;
  levelStream << "Character: Level " << player->getLevel();
  healthStream << "Health: " << player->getHealth() << "/" << player->getMaxHealth();

  damageStream << "Damage: " << player->getDamage();
  expStream << "Exp: " << player->getExp() << "/" << player->getLevelExperienceMax();


  m_pCharacterWnd->getChild("CharacterSheet/Health")->setText(healthStream.str());
  m_pCharacterWnd->getChild("CharacterSheet/Damage")->setText(damageStream.str());
  m_pCharacterWnd->getChild("CharacterSheet/Experience")->setText(expStream.str());
  m_pCharacterWnd->setText(levelStream.str());
}

bool GameState::onGameMenuButton(const CEGUI::EventArgs &args)
{
  const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(args);
  CEGUI::String buttonName = we.window->getName();
  if (buttonName.compare("GameMenu/ExitButton") == 0) {
      m_bQuit = true;
  }
  else if (buttonName.compare("GameMenu/Resume") == 0) {
    toggleGameMenu();
  }
  return true;
}

bool GameState::onGameHUDButton(const CEGUI::EventArgs &args)
{
  const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(args);
  CEGUI::String buttonName = we.window->getName();
  if (buttonName.compare("HUD/Skill1") == 0) {
    ToggleAttack();
  }
  else if (buttonName.compare("HUD/Skill1Pressed") == 0) {
    ToggleAttack();
  }
  return true;
}

bool GameState::onLootWindowClose(const CEGUI::EventArgs &args)
{
  DestroyAndHideLootWindow();
  return true;
}

bool GameState::onInventorySlotButton(const CEGUI::EventArgs &args)
{
  const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(args);
  std::string slotStr = we.window->getName().c_str();
  int btnNumber = atoi(slotStr.erase(0, 14).c_str());
  std::vector<Item*> inventory = player->getInventory();
  if((btnNumber-1) < inventory.size()) {
    std::cout << btnNumber << ": " << inventory[btnNumber-1]->getName() << std::endl;

    //USE ITEM
    Item* item = player->getItemByIndex(btnNumber-1);
    std::vector<Attribute*> attributes = item->getAttributes();
    bool removeAfterUse = true;
    bool shouldAddAttr = true;
    if(item->isEquipable()) {
        if ( player->getEquipItem(item->getSlot()) != NULL ) {
            shouldAddAttr = false;
            removeAfterUse = false;
        } else {
            player->setEquipItem(WEAPON, item);
        }
    }
    if (shouldAddAttr) {
        for(std::vector<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++) {
            if( (*it)->stat == ATTR_HEALTH ) {
            player->setHealth(player->getHealth() + (*it)->amount);
            }
        }
    }
    //REMOVE ITEM
    if (removeAfterUse)
    player->removeItemByIndex(btnNumber-1);
  }
  else
    std::cout << btnNumber << ": Empty" << std::endl;

  if ( player->getEquipItems()[1] )
    std::cout << "EQUIP: " << player->getEquipItems()[1]->getName() << std::endl;

  return true;
}

bool GameState::onCharEquipButton(const CEGUI::EventArgs &args)
{
  const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(args);
  CEGUI::String buttonName = we.window->getName();
  std::map<int, Item*> equipItems = player->getEquipItems();
  if (buttonName.compare("EquipItems/Weapon") == 0) {
    if(equipItems[WEAPON]) {
      std::cout << "Removing Weapon: " << equipItems[WEAPON]->getName() << std::endl;
      
      int slot = WEAPON;
      Item* item = player->getEquipItem(slot);
      std::vector<Attribute*> attributes = item->getAttributes();
      for(std::vector<Attribute*>::iterator it = attributes.begin(); it != attributes.end(); it++) {
        if( (*it)->stat == ATTR_HEALTH ) {
          player->setHealth(player->getHealth() - (*it)->amount);
        }
      }

      player->addItem(item);
      player->removeEquipItem(slot);
    }
  }
  else if (buttonName.compare("EquipItems/Helmet") == 0) {
    if(equipItems[HELMET])
      std::cout << "Helmet: " << equipItems[0]->getName() << std::endl;
  }
  else if (buttonName.compare("EquipItems/Boots") == 0) {
    if(equipItems[BOOTS])
      std::cout << "Boots: " << equipItems[2]->getName() << std::endl;
  }
  return true;
}

void GameState::buildGUI()
{
  OgreFramework::getSingletonPtr()->m_pGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow"); 
  CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow"); 
	const OIS::MouseState state = OgreFramework::getSingletonPtr()->m_pMouse->getMouseState();
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 
	CEGUI::System::getSingleton().injectMouseMove(state.X.abs-mousePos.d_x,state.Y.abs-mousePos.d_y);

  CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();
  m_pMainWnd = windMgr.getWindow("GAME_GUI");
  CEGUI::ProgressBar* playerExp = (CEGUI::ProgressBar*)m_pMainWnd->getChild("HUD/ExperienceBar");
  playerExp->setProgress(player->getExp()/(float)player->getLevelExperienceMax());
  
  m_pFpsWnd = windMgr.getWindow("FPS/Text");

  m_pGameMenuWnd = windMgr.getWindow("GameMenuWindow");
  CEGUI::PushButton* gameExitButton = (CEGUI::PushButton*)m_pGameMenuWnd->getChild("GameMenu/ExitButton");
  gameExitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onGameMenuButton, this));
  CEGUI::PushButton* gameResumeButton = (CEGUI::PushButton*)m_pGameMenuWnd->getChild("GameMenu/Resume");
  gameResumeButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onGameMenuButton, this));

  m_pInventoryWnd = windMgr.getWindow("InventoryWindow");
  std::stringstream ss;
  ss << "Inventory (" << player->getGold() << "g)";
  m_pInventoryWnd->setText(ss.str());
  createInventoryButtons();

  m_pCharacterWnd = windMgr.getWindow("CharacterSheetWindow");
  m_pCharacterWnd->getChild("CharacterSheet/Name")->setText("Name: "+player->getInGameName());

  m_pCharacterEquipWnd = m_pCharacterWnd->getChild("CharacterSheet/EquipItems");
  CEGUI::PushButton* weaponEquipBtn = (CEGUI::PushButton*)m_pCharacterEquipWnd->getChild("EquipItems/Weapon");
  weaponEquipBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onCharEquipButton, this));
  CEGUI::PushButton* helmetEquipBtn = (CEGUI::PushButton*)m_pCharacterEquipWnd->getChild("EquipItems/Helmet");
  helmetEquipBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onCharEquipButton, this));
  CEGUI::PushButton* bootsEquipBtn = (CEGUI::PushButton*)m_pCharacterEquipWnd->getChild("EquipItems/Boots");
  bootsEquipBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onCharEquipButton, this));

  m_pTargetWnd = windMgr.getWindow("HUD/TargetWindow");

  m_pLootWnd = windMgr.getWindow("LootWindow");
  m_pLootWnd->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&GameState::onLootWindowClose, this));

  CEGUI::PushButton* attackButtonUp = (CEGUI::PushButton*)m_pMainWnd->getChild("HUD/Skill1");
  attackButtonUp->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onGameHUDButton, this));
  CEGUI::PushButton* attackButtonDown = (CEGUI::PushButton*)m_pMainWnd->getChild("HUD/Skill1Pressed");
  attackButtonDown->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onGameHUDButton, this));
  CEGUI::PushButton* magicButtonUp = (CEGUI::PushButton*)m_pMainWnd->getChild("HUD/Skill2");
  magicButtonUp->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onGameHUDButton, this));
  CEGUI::PushButton* magicButtonDown = (CEGUI::PushButton*)m_pMainWnd->getChild("HUD/Skill2Cooldown");
  magicButtonDown->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onGameHUDButton, this));
  
  // Init player headers GUI
  printf("1\n");
  std::map<int, Player*> players = world->checkNewPlayers();
  printf("2\n");
  for(std::map<int, Player*>::iterator it = players.begin(); it!= players.end(); ++it) {
    FloatingText* ft = new FloatingText(m_pMainWnd, it->second, it->second->getName());
    ftexts.push_back(ft);
  }
  printf("3\n");

  OgreFramework::getSingletonPtr()->m_pGUISystem->setGUISheet(m_pMainWnd);

}

void GameState::SetButtonImage(CEGUI::Window* btn, std::string image)
{
  btn->setProperty("NormalImage", image);
  btn->setProperty("HoverImage", image);
  btn->setProperty("PushedImage", image);
}

void GameState::createInventoryButtons()
{
  int num_inv_slots = 36;
  int col = 0; 
  int row = 0;
  for(int i = 1; i <= num_inv_slots; i++) {
    std::stringstream ss;
    ss << "Inventory/Slot" << i;
    CEGUI::PushButton *tempBtn = (CEGUI::PushButton*)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button", ss.str());
    tempBtn->setPosition( CEGUI::UVector2( CEGUI::UDim( 0.05f + (0.15f * col), 0.0f), CEGUI::UDim( 0.05f + (0.15f * row), 0.0f ) ) );
    tempBtn->setSize( CEGUI::UVector2( CEGUI::UDim( 0.15f, 0.0f ), CEGUI::UDim( 0.15f, 0.0f ) ) );
    tempBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onInventorySlotButton, this));
    m_pInventoryWnd->addChildWindow(tempBtn);
    col++;
    if( i % 6 == 0) {
      col = 0;
      row++;
    }
  }
}

void GameState::clearInventoryButtons()
{
  int num_inv_slots = 36;
  for(int i = 1; i <= num_inv_slots; i++) {
    std::stringstream itemSlot;
    itemSlot << "Inventory/Slot" << i;
    CEGUI::Window* invSlot = m_pInventoryWnd->getChild(itemSlot.str());
    SetButtonImage(invSlot, "");
  }
}

void GameState::clearEquipButtons()
{
  SetButtonImage(m_pCharacterEquipWnd->getChild("EquipItems/Weapon"), "");
  SetButtonImage(m_pCharacterEquipWnd->getChild("EquipItems/Helmet"), "");
  SetButtonImage(m_pCharacterEquipWnd->getChild("EquipItems/Boots"), "");
}

//Returns True if the name is not part of the ignore cases
//QUICK FIX
bool GameState::isNotIgnoreCase(std::string name)
{
  if(name.compare("floorNode") == 0) { return false; }
  std::string testname = name.substr(0, 4);
  if (testname.compare("tree") == 0) return false;
  // testname = name.substr(0, 6);
  // if (testname.compare("player") == 0) return false;
  testname = name.substr(0, 2);
  if (testname.compare("bc") == 0) return false;
  return true;
}

bool GameState::getAndSendKeyInput()
{
  if(m_bSettingsMode == false) {

    Ogre::Vector3 step(0,0,0);
    Ogre::Quaternion q(Ogre::Degree(-90), Ogre::Vector3::UNIT_Y);
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
      step -= mDirection;
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
      step += mDirection;
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_E))
      step += (q*mDirection);
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Q))
      step -= (q*mDirection);
    step.normalise();
    step *= mMoveScale;

    Ogre::Radian rot(0.f);
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
      rot += Ogre::Radian(mRotateScale);
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
      rot -= Ogre::Radian(mRotateScale);
    Ogre::Quaternion quat(rot, Ogre::Vector3::UNIT_Y);
	  mDirection = quat * mDirection;

    float v = 0.1f;
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_SPACE))
      v += 10.f;

    bool playerMoved = !step.isZeroLength() || rot.valueDegrees() != 0;
    if (playerMoved)
      world->injectInput(step, rot, 0);

    return playerMoved;
  }

  return false;
}

void GameState::CreateAndShowLootWindow(Character* c)
{
  m_pLootWnd->setVisible(true);
  m_pLootWnd->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 200), CEGUI::UDim(0, 200)));
  CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();
  CEGUI::PushButton* tempBtn;
  std::stringstream ss;
  int i = 0;

  // first, add a fake item to represent gold
  if (c->getGold() > 0) {
    ss << "inv" << i;
    tempBtn = (CEGUI::PushButton*)windMgr.createWindow("TaharezLook/Button", ss.str());
    ss.str("");
    ss << c->getGold() << " Gold Pieces";
    tempBtn->setText(ss.str());
    tempBtn->setHorizontalAlignment(CEGUI::HA_CENTRE);
    tempBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 10+35*i)));
    tempBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.8, 0), CEGUI::UDim(0, 30)));
    tempBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onLootClick, this));
    m_pLootWnd->addChildWindow(tempBtn);
    i++;
  }

  // next, get the actual items
  std::vector<Item*> inv = c->getInventory();
  for (std::vector<Item*>::iterator it = inv.begin(); it != inv.end(); it++) {
    ss.str("");
    ss << "inv" << i;
    tempBtn = (CEGUI::PushButton*)windMgr.createWindow("TaharezLook/Button", ss.str());
    ss.str("");
    ss << (*it)->getName();
    tempBtn->setText(ss.str());
    tempBtn->setHorizontalAlignment(CEGUI::HA_CENTRE);
    tempBtn->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 10+35*i)));
    tempBtn->setSize(CEGUI::UVector2(CEGUI::UDim(0.8, 0), CEGUI::UDim(0, 30)));
    tempBtn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GameState::onLootClick, this));
    m_pLootWnd->addChildWindow(tempBtn);
    i++;
  }
}

bool GameState::onLootClick(const CEGUI::EventArgs &args)
{
  const CEGUI::MouseEventArgs& we = static_cast<const CEGUI::MouseEventArgs&>(args);
  std::string itemName = we.window->getText().c_str();
  DestroyLootSubwindow(we.window);

  int gIndex = itemName.find("Gold");
  if(gIndex <= itemName.length())
    itemName = itemName.substr(gIndex);

  if(itemName.compare("Gold Pieces") == 0) {
    player->setGold(player->getGold()+player->getTarget()->getGold());
    player->getTarget()->setGold(0);
  } else {
    Item* i = player->getTarget()->getItem(itemName);
    player->getTarget()->removeItem(itemName);
    player->addItem(i);
  }

  return true;
}

void GameState::DestroyAndHideLootWindow()
{
  m_pLootWnd->setVisible(false);
  int num = m_pLootWnd->getChildCount();
  for (int i = 0; i < num; i++) {
    std::stringstream ss;
    ss << "inv" << i;
    if (m_pLootWnd->isChild(ss.str()))
      DestroyLootSubwindow(m_pLootWnd->getChild(ss.str()));
  }
}

void GameState::DestroyLootSubwindow(CEGUI::Window* w)
{
  m_pLootWnd->removeChildWindow(w);
  CEGUI::WindowManager::getSingleton().destroyWindow(w);
}

void GameState::ToggleAttack()
{
  player->setAttacking(!player->isAttacking());
  CEGUI::PushButton* attackButtonPress = (CEGUI::PushButton*)m_pMainWnd->getChild("HUD/Skill1Pressed");
  CEGUI::PushButton* attackButtonUp = (CEGUI::PushButton*)m_pMainWnd->getChild("HUD/Skill1");
  attackButtonPress->setVisible(player->isAttacking());
  attackButtonUp->setVisible(!player->isAttacking());
}
