#ifndef __AdvancedOgreFramework_h__
#define __AdvancedOgreFramework_h__

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/CEGUIOgreRenderer.h>

class OgreFramework : public Ogre::Singleton<OgreFramework>, OIS::KeyListener, OIS::MouseListener
{
public:
	OgreFramework();
	~OgreFramework();

	bool initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener = 0, OIS::MouseListener *pMouseListener = 0);
	void updateOgre(double timeSinceLastFrame);

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	Ogre::Root*					  m_pRoot;
	Ogre::RenderWindow*		m_pRenderWnd;
	Ogre::Viewport*				m_pViewport;
	Ogre::Log*					  m_pLog;
	Ogre::Timer*				  m_pTimer;
	Ogre::String				  mResourcesCfg;
	Ogre::String				  mPluginsCfg;

	OIS::InputManager*		m_pInputMgr;
	OIS::Keyboard*				m_pKeyboard;
	OIS::Mouse*					  m_pMouse;

  OgreBites::SdkTrayManager*	m_pTrayMgr;
  CEGUI::OgreRenderer*        m_pGUIRenderer;
  CEGUI::System*              m_pGUISystem;

private:
	OgreFramework(const OgreFramework&);
	OgreFramework& operator= (const OgreFramework&);
};

#endif
