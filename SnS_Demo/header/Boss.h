#ifndef __Boss_h__
#define __Boss_h__

#include "Character.h"
#include "Player.h"

#define STAND 0
#define WANDER 1
#define SEEK 2
#define ATTACK 3
#define DEAD 4

class Boss :
  public Character
{
public:
  Boss(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName);
  Boss(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height);
  virtual ~Boss(void);
  virtual void update(Ogre::Real dt);
  void setTarget(Player* player);
  void addPlayerEnemy(Player* player);
  void removePlayerEnemy(Player* player);
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

  Ogre::Vector3 den;
};

#endif //__Boss_h__
