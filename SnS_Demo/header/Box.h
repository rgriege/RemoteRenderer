#ifndef __Box_h__
#define __Box_h__

#include "GameObject.h"

class Box :
  public GameObject
{
public:
  Box(Ogre::SceneManager* _sceneMgr, Ogre::Vector3 _halfExtents, Ogre::String _name);
  virtual ~Box(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);

protected:
  Ogre::Vector3 halfExtents;
};

#endif // __Box_h__
