#ifndef __OptionsMenu_h__
#define __OptionsMenu_h__

#include "AppState.h"

class OptionsMenu : public AppState
{
public:
	OptionsMenu();

	DECLARE_APPSTATE_CLASS(OptionsMenu)

	void enter();
	void createScene();
	void exit();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void buttonHit(OgreBites::Button* button);

	void update(double timeSinceLastFrame);

private:
	bool m_bQuit;
};

#endif
