#include "Enemy.h"
#include <Ogre.h>
#include <stdlib.h>
#include "ItemDatabase.h"

Enemy::Enemy(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, std::string _objName) : Character(_name, _sceneMgr, _height, _objName)
{
  int startRange = 8000;
  den = Ogre::Vector3(rand() % startRange - startRange/2, 0, 
                      rand() % startRange - startRange/2);
  setPosition(den);
  range = 200;
  state = STAND;
  printf("enemy %s created at %f %f\n", name.c_str(), den.x, den.z);
  target = NULL;
  maxHealth = 100;
  health = 100;
  damage = 5;
  attackspeed = 1500;
  experience = 50;
}

Enemy::Enemy(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height) : Character(_name, _sceneMgr, _height)
{
  int startRange = 8000;
  den = Ogre::Vector3(rand() % startRange - startRange/2, height/2, 
                      rand() % startRange - startRange/2);
  setPosition(den);
  range = 200;
  state = STAND;
  printf("enemy %s created at %f %f\n", name.c_str(), den.x, den.z);
  target = NULL;
  maxHealth = 100;
  health = 100;
  damage = 50;
  attackspeed = 1500;
  experience = 50;
}

Enemy::Enemy(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height, int _type) : Character(_name, _sceneMgr, _height)
{
  type = _type;
  if(type == CUBE)
  {
	  int startRange = 8000;
	  den = Ogre::Vector3(rand() % startRange - startRange/2, height/2, 
		              rand() % startRange - startRange/2);
	  setPosition(den);
	  range = 200;
	  state = STAND;
	  printf("enemy %s created at %f %f\n", name.c_str(), den.x, den.z);
	  target = NULL;
	  maxHealth = 100;
	  health = 100;
	  damage = 10;
	  attackspeed = 1500;
	  experience = 50;
  }
  else
  {
	  int startRange = 8000;
	  den = Ogre::Vector3(5000, height/2, 
		              5000);
	  setPosition(den);
	  range = 500;
	  state = STAND;
	  printf("Boss %s created at %f %f\n", name.c_str(), den.x, den.z);
	  target = NULL;
	  maxHealth = 1000;
	  health = 1000;
	  damage = 50;
	  attackspeed = 1500;
	  experience = 1000;
  }
}

Enemy::~Enemy(void)
{
}

void Enemy::setTarget(Player* player)
{
  target = player;
}

void Enemy::addPlayerEnemy(Player* player)
{
  playerEnemies[playerEnemies.size()] = player;
}

void Enemy::removePlayerEnemy(Player* player)
{
  if (target == player) {
    target = NULL;
    setState(WANDER);
  }
  for(std::map<int, Player*>::iterator it = playerEnemies.begin(); it != playerEnemies.end(); it++) {
    if (it->second == player) {
      playerEnemies.erase(it);
      break;
    }
  }
}

void Enemy::update(Ogre::Real dt)
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
      } 
      else if (targetDiff.length() <= 15 && type == CUBE) {
        //setVelocity(Ogre::Vector3::ZERO);
        setState(ATTACK);
      }
      else if (targetDiff.length() <= 80 && type == BOSS) {
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
      Ogre::Real targetDiff =  (getPosition() - target->getPosition()).length();
      if(type == CUBE)
      {
	      if(targetDiff <= 15 && attackspeed <= 0)
	      {
		printf("My Health: %d\n", target->getHealth());
		target->setHealth(target->getHealth() - damage);
		attackspeed = 1500;
	      }

	      else if(targetDiff > 15 && attackspeed < 1500)
	      {
		attackspeed = 1500;
		setState(SEEK);
	      }
	      else
		attackspeed -= dt;
      }
      else
      {
	      if(targetDiff <= 80 && attackspeed <= 0)
	      {
		printf("My Health: %d\n", target->getHealth());
		target->setHealth(target->getHealth() - damage);
		attackspeed = 1500;
	      }

	      else if(targetDiff > 80 && attackspeed < 1500)
	      {
		attackspeed = 1500;
		setState(SEEK);
	      }
	      else
		attackspeed -= dt;
      }
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

void Enemy::setState(int newState)
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

bool Enemy::nearTarget(Player* p)
{
  return p && getPosition().squaredDistance(p->getPosition()) < range * range;
}

bool Enemy::foundTarget()
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

int Enemy::getType()
{
	return type;
}

Ogre::Vector3 Enemy::getNewTargetLocation()
{
  Ogre::Vector3 result(den + Ogre::Vector3(rand() % 2*range - range, 0, 
                                           rand() % 2*range - range));
  while(getPosition().distance(result) < 0.3f*range)
  result = den + Ogre::Vector3(rand() % 2*range - range, 0, 
                         rand() % 2*range - range);
  return result;
}

void Enemy::faceTargetLocation()
{
  Ogre::Vector3 newDirection = targetLocation - this->getPosition();
  setDirection(newDirection);
}

void Enemy::generateLoot() {
  printf("generating random loot\n");
  if (rand() % 100 < 100)
    addItem(ItemDatabase::GetInstance()->getRandomItem());
  setGold(rand() % 4 + 2);
}
