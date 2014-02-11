#include "Player.h"
#include "Enemy.h"
#include <typeinfo>
#include <Ogre.h>

Player::Player(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName) : Character(_name, _sceneMgr, _height, _objName)
{
  yaw(Ogre::Radian(Ogre::Degree(90).valueRadians()));
  jumpUp = false;
  jumpDown = false;
  jumpTime = 0;
  attackspeed = 1000;
  maxHealth = 200;
  health = 200;
  damage = 10;
  attack = false;
  animatingAttack = false;
  attackTime = getAttackSpeed();
  target = NULL;
  spellAttack = false;
  levelExperienceMax = 500;
  mana = 200;
  maxMana = 200;

  animationState = entity->getAnimationState("my_animation");
  animationState->setLoop(true);
  animationState->setEnabled(true);
}

Player::Player(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height) : Character(_name, _sceneMgr, _height)
{
  setPosition(Ogre::Vector3(0, height/2, 0));
  yaw(Ogre::Radian(Ogre::Degree(90).valueRadians()));
  jumpUp = false;
  jumpDown = false;
  jumpTime = 0;
  attackspeed = 1000;
  maxHealth = 200;
  health = 200;
  damage = 10;
  attack = false;
  animatingAttack = false;
  attackTime = getAttackSpeed();
  target = NULL;
  spellAttack = false;
  levelExperienceMax = 500;
  mana = 200;
  maxMana = 200;

  animationState = NULL;
}

Player::~Player(void)
{
}

void Player::update(Ogre::Real dt)
{
  if (animationState)
    animationState->addTime(dt);
  // attack stuff
  if (target != NULL && typeid(*target) == typeid(Enemy)) {
    Ogre::Real distance = (getPosition() - target->getPosition()).length();
    if((distance <= 15 && attack && !animatingAttack && ((Enemy*)target)->getType() == CUBE) || (distance <= 50 && attack && !animatingAttack && ((Enemy*)target)->getType() == BOSS))
    {
      printf("Starting Attack\n");
      animatingAttack = true;
    }

    if(animatingAttack && attackTime <= 0 && target->getHealth() > 0)
    {
      printf("Finished Attack\n");    
      target->setHealth(target->getHealth() - getDamage());
      printf("Enemy health: %i\n", target->getHealth());
      animatingAttack = false;
      attackTime = attackspeed;
      if (target->getHealth() <= 0) {
        printf("Enemy has been defeated\n");
        target->hasAlreadyGivenExp();
        incrementExperience(target->getExp());
        target = NULL;
      }
    }
    else if (spellAttack && target->getHealth() <= 0 && !target->hasGivenExp()) {
      printf("Enemy has been defeated with spell\n");
      target->hasAlreadyGivenExp();
      incrementExperience(target->getExp());
      animatingAttack = false;
      attackTime = attackspeed;
      spellAttack = false;
      target = NULL;
    }
    else
      attackTime-= dt;
  }

}

int Player::getDamage()
{
  return (10 * pow(1.5, (level-1))) + getEquipDamage();
}

Ogre::Real Player::getHeight()
{
  return height;
}

void Player::setJumpUp(bool b)
{
  jumpUp = b;
}

bool Player::getJumpUp()
{
  return jumpUp;
}

void Player::setJumpDown(bool b)
{
  jumpDown = b;
}

bool Player::getJumpDown()
{
  return jumpDown;
}

void Player::setJumpTime(Ogre::Real t)
{
  jumpTime = t;
}

Ogre::Real Player::getJumpTime()
{
  return jumpTime;
}

void Player::jump(float vel)
{
  bool spacebar = vel >= 10;
  vel = (spacebar ? vel - 10 : vel);
  if (spacebar && !jumpUp && !jumpDown)
    jumpUp = true;
  if (jumpUp && jumpTime < 100) {
    this->translate(Ogre::Vector3(0,vel,0));
    jumpTime += 1;
  } else {
    jumpUp = false;
    jumpDown = true;
  }
  if (jumpDown && jumpTime > 0) {
    this->translate(Ogre::Vector3(0,-vel,0));
    jumpTime -= 1;
  } else {
    jumpDown = false;
  }

  /*if (body->getCenterOfMassPosition().y() < 0.1f) {
    body->applyCentralImpulse(btVector3(0,20,0));
    printf("jumped! %f\n", body->getLinearVelocity().y());
    grounded = false;
  }*/
}

void Player::setMana(int m)
{
  mana = m;
}

int Player::getMana()
{
  return mana;
}

int Player::getMaxMana()
{
  return maxMana;
}

void Player::levelup()
{
  setLevel(level+1);
  setDamage((int)(damage * 1.5f));
  setExp(levelExperienceMax - experience);
  setLevelExperienceMax(levelExperienceMax * 2);
  setMaxHealth(maxHealth * 1.5);
  setHealth((int)maxHealth);
}

int Player::getLevelExperienceMax()
{
  return levelExperienceMax;
}

void Player::setLevelExperienceMax(int _maxExperience)
{
  levelExperienceMax = _maxExperience;
}

bool Player::isAttacking()
{
  return attack;
}

void Player::setAttacking(bool attacking)
{
  attack = attacking;
}

bool Player::isSpellAttacking()
{
  return spellAttack;
}

void Player::setSpellAttacking(bool _spellAttack)
{
  spellAttack = _spellAttack;
}

void Player::incrementExperience(int _experience)
{
  experience += _experience;
  changes = changes | (1 << EXP_CHANGE);
  if(experience >= levelExperienceMax) {
    levelup();
  }
}

Character* Player::getTarget()
{
  return target;
}

void Player::setTarget(Character* _target)
{
  target = _target;
}

