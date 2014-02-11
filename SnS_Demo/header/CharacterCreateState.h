#ifndef CHARACTER_CREATE_STATE_H
#define CHARACTER_CREATE_STATE_H

#include "AppState.h"
#include "pugixml.hpp"

class CharacterCreateState : public AppState
{
public:
	DECLARE_APPSTATE_CLASS(CharacterCreateState)

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

  bool onCreateCharacterButton(const CEGUI::EventArgs &args);

	void update(double timeSinceLastFrame);

  void buildGUI();

private:
	bool						m_bQuit;

  CEGUI::Window*  m_pMainWnd;
  CEGUI::Window*  m_pCharWnd;
  
  pugi::xml_document doc;
  pugi::xml_node root;
};

#endif
