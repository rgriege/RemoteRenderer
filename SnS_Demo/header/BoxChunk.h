#ifndef __BoxChunk_h__
#define __BoxChunk_h__

#include "GameObject.h"
#include "Character.h"

class BoxChunk :
  public GameObject
{
public:
  BoxChunk(Ogre::SceneManager* _sceneMgr, Character* _character);
  virtual ~BoxChunk(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);
  virtual void update(Ogre::Real dt);
  bool finished();

private:
  Ogre::Real height;
  Ogre::Real speed;
  Ogre::Vector3 direction;

  static Ogre::String nextName;
  static Ogre::Vector3 gravity;
  static int nextNum;
};

#endif //__BoxChunk_h__
