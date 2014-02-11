#ifndef __Plane_h__
#define __Plane_h__

#include "GameObject.h"

class Plane :
  public GameObject
{
public:
  Plane(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Vector3 _normal, Ogre::Vector3 _upVector, Ogre::Real _coord);
  virtual ~Plane(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);
  Ogre::Vector3 getNormal();
protected:
  Ogre::Vector3 normal;
  Ogre::Vector3 upVector;
  Ogre::Real coord;

};

#endif // __Plane_h__
