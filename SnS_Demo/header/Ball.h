#ifndef __Ball_h__
#define __Ball_h__

#include "GameObject.h"

class Ball :
  public GameObject
{
public:
  Ball(Ogre::SceneManager* _sceneMgr, Ogre::Real _radius, Ogre::String _name);
  virtual ~Ball(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);
  Ogre::Real getRadius();
protected:
  Ogre::Real radius;
};

#endif // __Ball_h__
