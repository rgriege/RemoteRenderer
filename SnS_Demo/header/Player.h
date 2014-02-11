#ifndef __Player_h__
#define __Player_h__

#include "Character.h"

class Player :
  public Character
{
public:
  Player(Ogre::String name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName);
  Player(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height);
  virtual ~Player(void);
  void update(Ogre::Real dt);
  Ogre::Real getHeight();

  void levelup();
  
  int getDamage();
  bool getJumpDown();
  bool getJumpUp();
  int getLevelExperienceMax();
  void setJumpUp(bool b);
  void setJumpDown(bool b);
  void setLevelExperienceMax(int _maxExperience);

  Ogre::Real getJumpTime();
  void setJumpTime(Ogre::Real t);
  bool isGrounded();
  void jump(float spacebar);

  void setMana(int m);
  int getMana();
  int getMaxMana();

  bool isAttacking();
  void setAttacking(bool attacking);

  bool isSpellAttacking();
  void setSpellAttacking(bool _spellAttack);

  void incrementExperience(int _experience);

  Character* getTarget();
  void setTarget(Character* _target);

protected:
  Ogre::Real jumpTime;
  bool jumpUp;
  bool jumpDown;
  Character* target;

  bool attack;
  int attackTime;
  bool animatingAttack;
  bool spellAttack;
  int mana;
  int maxMana;


  int levelExperienceMax;
};

#endif //__Player_h__
