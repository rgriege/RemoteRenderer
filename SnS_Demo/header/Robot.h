#ifndef __Robot_h__
#define __Robot_h__

#include "Enemy.h"
#include "Player.h"

class Robot :
  public Enemy
{
public:
  Robot(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height);
  virtual ~Robot(void);
  void update(Ogre::Real dt);
  void setState(int newState);
};

#endif //__Robot_h__
