#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "AppState.h"

class MenuState : public AppState
{
public:
	DECLARE_APPSTATE_CLASS(MenuState)

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

	bool onExitButton(const CEGUI::EventArgs &args);
	bool onEnterButton(const CEGUI::EventArgs &args);

	void update(double timeSinceLastFrame);

  void buildGUI();

private:
	bool						m_bQuit;
};

#endif
