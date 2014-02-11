#ifndef __Enemy_h__
#define __Enemy_h__

#include "Character.h"
#include "Player.h"

#define STAND 0
#define WANDER 1
#define SEEK 2
#define ATTACK 3
#define DEAD 4
#define CUBE 5
#define BOSS 6

class Enemy :
  public Character
{
public:
  Enemy(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName);
  Enemy(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height);
  Enemy(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, int _type);
  virtual ~Enemy(void);
  virtual void update(Ogre::Real dt);
  void setTarget(Player* player);
  void addPlayerEnemy(Player* player);
  void removePlayerEnemy(Player* player);
  int getType();
  virtual void setState(int newState);

protected:
  bool nearTarget(Player* player);
  bool foundTarget();
  Ogre::Vector3 getNewTargetLocation();
  void faceTargetLocation();
  void generateLoot();

  Player* target;
  std::map<int, Player*> playerEnemies;
  Ogre::Vector3 targetLocation;
  int state;
  int type;

  Ogre::Vector3 den;
};

#endif //__Enemy_h__
