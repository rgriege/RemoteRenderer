#ifndef __SoundMenu_h__
#define __SoundMenu_h__

#include "AppState.h"

class SoundMenu : public AppState
{
public:
    SoundMenu();

	DECLARE_APPSTATE_CLASS(SoundMenu)

	void enter();
	void createScene();
	void exit();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void buttonHit(OgreBites::Button* button);
  void sliderMoved(OgreBites::Slider* slider);

	void update(double timeSinceLastFrame);

private:
	bool m_bQuit;
};

#endif