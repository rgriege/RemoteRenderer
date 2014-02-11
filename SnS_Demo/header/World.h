#ifndef __World_h__
#define __World_h__

#include "btBulletDynamicsCommon.h"
#include "GameObject.h"
#include "RemoteOgreFramework.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Robot.h"
#include "Character.h"
#include "Projectile.h"
#include "pugixml.hpp"
#include <string>
#include <vector>

#define NUM_TREES 500

class World
{
public:
  World(Ogre::SceneManager* m_pSceneMgr);
  World(Ogre::SceneManager* m_pSceneMgr, void* data);
  virtual ~World(void);
  void loadMap();
  void addObject(GameObject *obj);
  void reset();
  void step(Ogre::Real dt);
  void injectInput(Ogre::Vector3 step, Ogre::Radian rad, int player);
  Character* getObjectByName(std::string name);

  void addPlayer(int idx, std::string name);
  Player* getPlayer(int idx);
  void removePlayer(int idx);
  void setPlayer(pugi::xml_node data, int idx);

  void addEnemy(int idx);

  void addProjectile(char spell, Player* player);
  void addProjectile(int idx);

  void* GetData();
  bool CanSetData(void* data);
  bool SetData(void* data);

  void SetPaused(bool pause);

  std::vector<Character*> checkCharacterHealth();
  bool checkPlayerGold(int idx);
  std::map<int, Player*> checkNewPlayers();

  friend bool HandleContacts(btManifoldPoint& point, btCollisionObject* body0, btCollisionObject* body1);
protected:
  btDiscreteDynamicsWorld *btWorld;
  Ogre::SceneManager* sceneMgr;
  std::map<int, Player*> players;
  std::map<int, Character*> chars;
  std::map<int, Projectile*> projectiles;  

  bool paused;
  bool newPlayer;
};


#endif // __World_h__
