#ifndef __Character_h__
#define __Character_h__

#include "GameObject.h"
#include <vector>
#include "pugixml.hpp"
#include "Item.h"

#define POSITION_CHANGE 7
#define ROTATION_CHANGE 6
#define HEALTH_CHANGE 5
#define INVENTORY_CHANGE 4
#define LEVEL_CHANGE 3
#define EXP_CHANGE 2
#define GOLD_CHANGE 1
#define EQUIP_CHANGE 0

#define HELMET 0
#define WEAPON 1
#define BOOTS 2

class Character :
  public GameObject
{
public:
  Character(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName);
  Character(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height);
  virtual ~Character(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);
  virtual void update(Ogre::Real dt);
  void setMesh(std::string _meshName);
  Ogre::Real getHeight();

  virtual void translate(const Ogre::Vector3 displacement);
  virtual void setPosition(const Ogre::Vector3 position);
  virtual void yaw(const Ogre::Radian radians);
  virtual void setYaw(const Ogre::Radian radians);

  void loadXML(pugi::xml_node node);

  int getDamage();
  int getHealth();
  int getExp();
  int getAttackSpeed();
  unsigned int getLevel();
  float getMaxHealth();
  int getGold();
  std::string getInGameName();
  int getEquipDamage();
  void setHealth(int _health);
  void setDamage(int _damage);
  void setExp(int _exp);
  void setAttackSpeed(int _attackspeed);
  void setMaxHealth(float _maxhealth);
  void setGold(int _gold);
  void setInGameName(std::string _inGameName);
  void setLevel(unsigned int _level);
  bool isDead();
  bool isFinished();
  void hasAlreadyGivenExp();
  bool hasGivenExp();
  std::vector<Item*> getInventory();
  void addItem(Item* item);
  void addItem(int idx);
  Item* getItem(std::string name);
  Item* getItemByIndex(int index);
  void removeItem(std::string name);
  void removeItemByIndex(int index);
  std::map<int, Item*> getEquipItems();
  Item* getEquipItem(int index);
  void setEquipItem(int index, Item* item);
  void removeEquipItem(int index);
  void clearEquipItems();
  void clearInventory();
  unsigned char getChanges();
  void clearChanges();
  void changeAll();

  int healthChange;
  bool updated;
protected:
  Ogre::Real height;
  Ogre::Real speed;

  std::string inGameName;

  unsigned int level;
  int damage;
  int health;
  int experience;
  int attackspeed;
  int range;
  float maxHealth;
  bool givenExp;
  int gold;

  bool finished;
  int deathTimer;

  unsigned char changes;

  std::vector<Item*> inventory;
  std::map<int, Item*> equipItems;

  Ogre::AnimationState* animationState;

private:
  Ogre::MaterialPtr mat;

  std::string obj;
  std::string mesh;

/* For Later Use
  int strength;
  int stamina;
  int intellect;
*/

};

#endif //__Character_h__
