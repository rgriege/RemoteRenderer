#include "Projectile.h"
#include "MotionState.h"
#include <Ogre.h>
#include <sstream>

Ogre::String Projectile::nextName = "p0";
int Projectile::nextNum = 1;

Projectile::Projectile(Ogre::SceneManager* _sceneMgr, Ogre::Vector3 start, Character* _target, int _spell) : GameObject(nextName)
{
  if(_spell == DEATHCOIL)
  {
	  height = 5.f;
	  speed = 0.5f;
	  mesh = "ogrehead.mesh";
	  damage = 30;
  }
  else
  {
	  height = 0;
	  speed = 0;
	  mesh = "";
	  damage = 0;
  }
  spell = _spell;
  target = _target;
  contactWithTarget = false;
  active = true;
  index = nextNum++;

  if (_sceneMgr) {
    sceneMgr = _sceneMgr;
    addToGraph(sceneMgr);
    //node->translate(Ogre::Vector3(0, height/2.f, 0));
  }

  // set up Bullet properties
  btTransform* transform = new btTransform(btMatrix3x3::getIdentity());
  MotionState *motionState = new MotionState(node, *transform);

  btCollisionShape *shape = new btBoxShape(btVector3(height/2, height/2, height/2));

  btScalar mass(1.f);
  btVector3 localInertia(0,0,0);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
  body = new btRigidBody(rbInfo);
  body->setLinearFactor(btVector3(1.f,0.f,1.f));
  body->forceActivationState(DISABLE_DEACTIVATION);
  body->setRestitution(1.f);
  body->setFriction(0.f);
  body->setUserPointer((void *)this);
  body->translate(btVector3(0, height/2.f, 0));

  translate(start);
  std::stringstream ss;
  ss << "p" << index;
  nextName = ss.str();
}

Projectile::~Projectile(void)
{
  if (node) {
    node->detachObject(entity);
    sceneMgr->destroyEntity(entity);
    sceneMgr->destroySceneNode(node);
  }
  if (target && target->getHealth() > 0)
    target->setHealth(target->getHealth() - damage);
}

void Projectile::addToGraph(Ogre::SceneManager* sceneMgr)
{
  // set up Ogre properties
  entity = sceneMgr->createEntity(name, mesh);

  node = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
  node->attachObject(entity);

  Ogre::Real cur_height = entity->getBoundingBox().getSize().y;
  node->scale(height/cur_height, height/cur_height, height/cur_height);
}

void Projectile::setMesh(std::string _meshName)
{
  mesh = _meshName;
}

Ogre::Real Projectile::getHeight()
{
  return height;
}

void Projectile::setPosition(const Ogre::Vector3 position)
{
  if (node) {
    Ogre::Vector3 diff = node->getPosition() - position;
    node->setPosition(position);
    body->translate(btVector3(diff.x, diff.y, diff.z));
  } else {
    btTransform transform = body->getCenterOfMassTransform();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    body->setCenterOfMassTransform(transform);
  }
  active = true;
}

void Projectile::update(Ogre::Real dt)
{
  Ogre::Vector3 targetDiff = target->getPosition() - this->getPosition();
  if (targetDiff.length() <= 10) {
    contactWithTarget = true;
  } else {
    Ogre::Vector3 newDirection = target->getPosition() - this->getPosition();
    setDirection(newDirection);
    newDirection.normalise();
    this->translate(newDirection * speed * dt);
  }
}

bool Projectile::getContactWithTarget()
{
	return contactWithTarget;
}

int Projectile::getSpell()
{
	return spell;
}

bool Projectile::isActive()
{
  return active;
}

void Projectile::clearStatus()
{
  active = false;
}