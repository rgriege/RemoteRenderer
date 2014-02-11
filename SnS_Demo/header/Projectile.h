#ifndef __Projectile_h__
#define __Projectile_h__

#include "GameObject.h"
#include "Character.h"

#define DEATHCOIL 0

class Projectile :
  public GameObject
{
public:
  Projectile(Ogre::SceneManager* _sceneMgr, Ogre::Vector3 start, Character* _target, int _spell);
  virtual ~Projectile(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr);
  virtual void setPosition(const Ogre::Vector3 position);
  virtual void update(Ogre::Real dt);
  void setMesh(std::string _meshName);
  Ogre::Real getHeight();
  bool getContactWithTarget();
  int getSpell();
  bool isActive();
  void clearStatus();

  int index;

private:
  std::string mesh;
  Ogre::Real height;
  Ogre::Real speed;
  static Ogre::String nextName;
  static int nextNum;
  Character* target;
  int damage;
  int spell;
  bool contactWithTarget;
  bool active;
};

#endif //__Projectile_h__
