#ifndef __GameState_h__
#define __GameState_h__

#include "AppState.h"
#include <vector>
#include <map>

#include "World.h"
#include "FloatingNumber.h"
#include "FloatingText.h"
#include "SoundManager.h"
#include "BoxChunk.h"
#include "Projectile.h"
#include "btBulletDynamicsCommon.h"
#include <pugixml.hpp>

class GameState : public AppState
{
public:
	GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	bool getAndSendKeyInput();
  void buildGUI();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

  bool onGameMenuButton(const CEGUI::EventArgs &args);
  bool onGameHUDButton(const CEGUI::EventArgs &args);
  bool onInventorySlotButton(const CEGUI::EventArgs &args);
  bool onLootWindowClose(const CEGUI::EventArgs &args);

  bool onCharEquipButton(const CEGUI::EventArgs &args);

	virtual void update(double timeSinceLastFrame);

  static bool m_bPause;
  static pugi::xml_node playerData;

private:
  void toggleGameMenu();
  void toggleInventory();
  void toggleCharacterSheet();
  bool isNotIgnoreCase(std::string name);

  void createInventoryButtons();
  void clearInventoryButtons();
  void clearEquipButtons();

  void CreateAndShowLootWindow(Character* c);
  void DestroyAndHideLootWindow();
  void DestroyLootSubwindow(CEGUI::Window* w);
  bool onLootClick(const CEGUI::EventArgs &args);
  void ToggleAttack();

  void SetButtonImage(CEGUI::Window* btn, std::string image);

private:
	Ogre::Real mRotate;          // The rotate constant
  Ogre::Real mMove;            // The movement constant
	Ogre::Real mZoom;			 // The zoom constant
  Ogre::Vector3 mDirection;    // Value to move in the correct direction
  Ogre::RaySceneQuery* mRayScnQuery;

	float mMoveScale;
  float mRotateScale;
	bool m_bQuit;
	bool m_bSettingsMode;
  bool m_bPan;
  bool menuToggle;
  bool inventoryToggle;
  bool characterToggle;

  bool deathCoilCoolDown;
  int deathCoilCounter;
  int deathCoilRange;
  int deathCoilCost;

  bool healCoolDown;
  int healCounter;
  int healRange;
  int healCost;

  int manaRegen;
  int manaRegenCounter;
  bool manaRegenFlag;

  std::vector<FloatingNumber*> fnums;
  std::vector<BoxChunk*> chunks;
  std::vector<FloatingText*> ftexts;

  World* world;
  Player* player;

  int timeSinceLastInput;

  CEGUI::Window*  m_pMainWnd;
  CEGUI::Window*  m_pFpsWnd;  
  CEGUI::Window*  m_pGameMenuWnd;
  CEGUI::Window*  m_pTargetWnd;
  CEGUI::Window*  m_pInventoryWnd;
  CEGUI::Window*  m_pCharacterWnd;
  CEGUI::Window*  m_pLootWnd;
  CEGUI::Window*  m_pCharacterEquipWnd;
};

#endif
