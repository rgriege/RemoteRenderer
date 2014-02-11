#ifndef CHARACTER_SELECT_STATE_H
#define CHARACTER_SELECT_STATE_H

#include "AppState.h"
#include "pugixml.hpp"

class CharacterSelectState : public AppState
{
public:
	DECLARE_APPSTATE_CLASS(CharacterSelectState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id); 
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

  bool onCharacterSelButton(const CEGUI::EventArgs &args);
  bool onCharacterCreateButton(const CEGUI::EventArgs &args);
  bool onEnterWorldButton(const CEGUI::EventArgs &args);

  bool SetPlayerInGameState();

	void update(double timeSinceLastFrame);

  void CreateCharacterButtons(std::string data);
  void ParseDatabase();
  void buildGUI();

private:
	bool m_bQuit;
  CEGUI::String character;

  CEGUI::Window*  m_pMainWnd;
  CEGUI::Window*  m_pCharWnd;

  std::string charactersData;
  
  pugi::xml_document doc;
  pugi::xml_node root;
};

#endif
