#include "Boss.h"
#include <Ogre.h>
#include <stdlib.h>
#include "ItemDatabase.h"

Boss::Boss(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName) : Character(_name, _sceneMgr, _height, _objName)
{
  int startRange = 8000;
  //den = Ogre::Vector3(rand() % startRange - startRange/2, 0, 
//                      rand() % startRange - startRange/2);
 // setPosition(den);
  range = 500;
  state = STAND;
  target = NULL;
  maxHealth = 1000;
  health = 1000;
  damage = 50;
  attackspeed = 1500;
  experience = 1000;
}

Boss::Boss(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height) : Character(_name, _sceneMgr, _height)
{
  int startRange = 8000;
  //den = Ogre::Vector3(rand() % startRange - startRange/2, 0, 
//                      rand() % startRange - startRange/2);
 // setPosition(den);
  range = 500;
  state = STAND;
  target = NULL;
  maxHealth = 1000;
  health = 1000;
  damage = 50;
  attackspeed = 1500;
  experience = 1000;
}

Boss::~Boss(void)
{
}

void Boss::setTarget(Player* player)
{
  target = player;
}

void Boss::addPlayerEnemy(Player* player)
{
  playerEnemies[playerEnemies.size()] = player;
}

void Boss::removePlayerEnemy(Player* player)
{
  for(std::map<int, Player*>::iterator it = playerEnemies.begin(); it != playerEnemies.end(); it++) {
    if (it->second == player) {
      playerEnemies.erase(it);
      break;
    }
  }
}

void Boss::update(Ogre::Real dt)
{
  switch(state) {
    case STAND:
      if (foundTarget())
        setState(SEEK);
      else if (rand() % 400 == 1)
        setState(WANDER);
      if (health <= 0)
        setState(DEAD);
      break;
    case WANDER:
    {
      Ogre::Real dist = targetLocation.distance(this->getPosition());
      if (dist < speed * dt) {
        setPosition(targetLocation);
        if (rand() % 5 != 1)
          setState(STAND);
        else
          setState(WANDER);
      } else {
        Ogre::Vector3 dir = (targetLocation - getPosition()).normalisedCopy();
        this->translate(dir * speed * dt);
      }
      if (foundTarget())
        setState(SEEK);
      if (health <= 0)
        setState(DEAD);
      break;
    }
    case SEEK:
    {
      targetLocation = target->getPosition();
      faceTargetLocation();
      Ogre::Vector3 dir = (targetLocation - getPosition()).normalisedCopy();
      dir.y = 0;
      this->translate(dir * speed * dt);
      Ogre::Vector3 targetDiff = target->getPosition() - this->getPosition();
      if (targetDiff.length() > range) {
        setState(WANDER);
      } else if (targetDiff.length() <= 10) {
        //setVelocity(Ogre::Vector3::ZERO);
        setState(ATTACK);
      }

      if (health <= 0)
        setState(DEAD);
      break;
    }
    case ATTACK:
    {
      targetLocation = target->getPosition();
      faceTargetLocation();
      Ogre::Vector3 targetDiff = target->getPosition() - this->getPosition();
      if(targetDiff.length() <= 30 && attackspeed <= 0)
      {
        printf("My Health: %d\n", target->getHealth());
        target->setHealth(target->getHealth() - damage);
        attackspeed = 1500;
      }

      else if(targetDiff.length() > 30 && attackspeed < 1500)
      {
        attackspeed = 1500;
        setState(SEEK);
      }
      else
        attackspeed -= dt;

      if (health <= 0)
        setState(DEAD);
      break;
    }
    case DEAD:
    {
      deathTimer++;
     // if (deathTimer == 1500)
        //finished = true;
      break;
    }
  }
}

void Boss::setState(int newState)
{
  if (newState == WANDER) {
    targetLocation = getNewTargetLocation();
    faceTargetLocation();
  } else if (newState == DEAD) {
    this->translate(Ogre::Vector3(0, -height * 3 / 4, 0));
    generateLoot();
  }
  state = newState;
}

bool Boss::nearTarget(Player* p)
{
  return p && getPosition().squaredDistance(p->getPosition()) < range * range;
}

bool Boss::foundTarget()
{
  for(std::map<int, Player*>::iterator it = playerEnemies.begin(); it != playerEnemies.end(); it++) 
  {
    if(nearTarget(it->second))
    {
      setTarget(it->second);
      return true;
    }
  }
  return false;
}

Ogre::Vector3 Boss::getNewTargetLocation()
{
  Ogre::Vector3 result(den + Ogre::Vector3(rand() % 2*range - range, 0, 
                                           rand() % 2*range - range));
  while(getPosition().distance(result) < 0.3f*range)
  result = den + Ogre::Vector3(rand() % 2*range - range, 0, 
                         rand() % 2*range - range);
  return result;
}

void Boss::faceTargetLocation()
{
  Ogre::Vector3 newDirection = targetLocation - this->getPosition();
  setDirection(newDirection);
}

void Boss::generateLoot() {
  printf("generating random loot\n");
  if (rand() % 100 < 100)
    addItem(ItemDatabase::GetInstance()->getRandomItem());
  setGold(rand() % 4 + 2);
}
