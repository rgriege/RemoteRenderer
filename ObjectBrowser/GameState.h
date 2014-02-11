#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_

#include <AppState.h>

class GameState : public AppState
{
public:
    GameState();

    DECLARE_APPSTATE_CLASS(GameState)

    virtual void enter();
    virtual void exit();
    virtual bool pause();
    virtual void resume();

    virtual bool keyPressed(const OIS::KeyEvent &keyEventRef);
    virtual bool keyReleased(const OIS::KeyEvent &keyEventRef);

    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    virtual void update(double timeSinceLastFrame);

private:
    bool mQuit;

    int mZoomScale;
    bool mZooming;
    bool mRotating;

    Ogre::Vector3 mMouseDownPos;
    Ogre::Quaternion mMouseDownOrientation;
    Ogre::Vector3 mMouseDownScale;

    Ogre::SceneNode* mHeadNode;
    Ogre::SceneNode* mCameraNode;
};

#endif