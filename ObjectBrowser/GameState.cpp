#include "GameState.h"

Ogre::Vector3 toSphere(Ogre::RenderTarget* wnd, float x, float y)
{
    float sx = (x - wnd->getWidth()/2)/(wnd->getWidth()/2);
    float sy = -(y - wnd->getHeight()/2)/(wnd->getHeight()/2);
    float pows = pow(sx, 2) + pow(sy, 2);
    return (pows > 1) ? Ogre::Vector3::ZERO : Ogre::Vector3(sx, sy, sqrt(1-pows));
}

GameState::GameState()
    : mZoomScale(30),
      mRotating(false),
      mZooming(false)
{
    
}

void GameState::enter()
{
    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC);

    Ogre::Entity* ogreHead = m_pSceneMgr->createEntity("Head", "ogrehead.mesh");
    mHeadNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    mHeadNode->attachObject(ogreHead);

    Ogre::Light* light = m_pSceneMgr->createLight();
    light->setPosition(20, 80, 50);
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

    m_pCamera = m_pSceneMgr->createCamera("MainCamera");
    m_pCamera->setNearClipDistance(5);
    m_pCamera->setPosition(0, 0, 80);
    m_pCamera->setAutoTracking(true, mHeadNode);
    mCameraNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    mCameraNode->attachObject(m_pCamera);

    OgreFramework::getSingletonPtr()->m_pViewport->setBackgroundColour(Ogre::ColourValue::Black);
    m_pCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth())
        / Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));
    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
}

bool GameState::pause()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");
    return true;
}

void GameState::resume()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    mQuit = false;
}
void GameState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");
    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

void GameState::update(double timeSinceLastFrame)
{
    if(mQuit == true)
        shutdown();
}

bool GameState::mouseMoved( const OIS::MouseEvent &arg )
{
    //std::cout << "Mouse Moved!" << std::endl;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(arg);
    if (mRotating) {
        Ogre::Vector3 currentMousePos = toSphere(OgreFramework::getSingletonPtr()->m_pRenderWnd, arg.state.X.abs, arg.state.Y.abs);
        Ogre::Vector3 rotationAxis = mMouseDownPos.crossProduct(currentMousePos);
        rotationAxis.normalise();
        Ogre::Radian rotationAngle = Ogre::Math::ACos(
            currentMousePos.dotProduct(mMouseDownPos) /
            (currentMousePos.length()*mMouseDownPos.length()));
        mHeadNode->setOrientation(mMouseDownOrientation);
        mHeadNode->rotate(rotationAxis, rotationAngle, Ogre::Node::TS_WORLD);
    }
    if (mZooming) {
        Ogre::Vector3 currentMousePos = Ogre::Vector3(0, arg.state.Y.abs, 0);
        mHeadNode->setScale(mMouseDownScale +
            Ogre::Vector3((arg.state.Y.abs - mMouseDownPos.y) / mZoomScale));
    }
    return true;
}

bool GameState::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    std::cout << "Mouse Pressed!" << std::endl;
    switch(id) {
    case OIS::MB_Left:
        mMouseDownPos = toSphere(OgreFramework::getSingletonPtr()->m_pRenderWnd, arg.state.X.abs, arg.state.Y.abs);
        mMouseDownOrientation = mHeadNode->getOrientation();
        mRotating = true;
        break;
    case OIS::MB_Right:
        mMouseDownPos = Ogre::Vector3(0, arg.state.Y.abs, 0);
        mMouseDownScale = mHeadNode->getScale();
        mZooming = true;
        break;
    }
    return true;
}

bool GameState::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    std::cout << "Mouse Released!" << std::endl;
    switch(id) {
    case OIS::MB_Left:
        mRotating = false;
        break;
    case OIS::MB_Right:
        mZooming = false;
        break;
    }
    return true;
}

bool GameState::keyPressed(const OIS::KeyEvent &arg)
{
    std::cout << "Key Pressed!" << std::endl;
    switch(arg.key) {
    case OIS::KC_R:
        mHeadNode->resetOrientation();
        mHeadNode->setScale(Ogre::Vector3::UNIT_SCALE);
        break;
    case OIS::KC_ESCAPE:
        mQuit = true;
        break;
    }
    return true;
}

bool GameState::keyReleased(const OIS::KeyEvent &arg)
{
    std::cout << "Key Released!" << std::endl;
    return true;
}