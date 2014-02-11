#ifndef __Blob_h__
#define __Blob_h__

#include "GameObject.h"

class Blob :
  public GameObject
{
public:
  Blob(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _radius);
  virtual ~Blob(void);
  Ogre::Real getRadius();
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);
  bool getJumpUp();
  void setJumpUp(bool b);
  bool getJumpDown();
  void setJumpDown(bool b);
  Ogre::Real getJumpTime();
  void setJumpTime(Ogre::Real t);
  bool isGrounded();
  void jump(float spacebar);

private:
  btCollisionShape* GenerateShape(float scale);

protected:
  Ogre::Real radius;
  Ogre::Real jumpTime;
  bool jumpUp;
  bool jumpDown;
};

#endif //__Blob_h__
