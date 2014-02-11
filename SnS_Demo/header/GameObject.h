#ifndef __GameObject_h__
#define __GameObject_h__

#include "stdafx.h"
#include <OgreEntity.h>
#include <btBulletDynamicsCommon.h>
#include <string>

class GameObject
{
public:
  GameObject(Ogre::String _name);
  virtual ~GameObject(void);
  virtual void addToGraph(Ogre::SceneManager* _sceneMgr) = 0;
  virtual void translate(const Ogre::Vector3 displacement);
  virtual Ogre::Vector3 getPosition();
  virtual void setPosition(const Ogre::Vector3 position);
  virtual Ogre::Radian getYaw();
  virtual void yaw(const Ogre::Radian radians);
  virtual void setYaw(const Ogre::Radian radians);
  virtual Ogre::Vector3 getDirection();
  virtual void setDirection(const Ogre::Vector3 direction);
  virtual Ogre::Vector3 getVelocity();
  virtual void setVelocity(Ogre::Vector3 _velocity);
  virtual btRigidBody* getRigidBody(void);
  virtual Ogre::String getName();
  virtual void showBoundingBox(bool show);

  static btCollisionShape* GenerateShape(std::string file, float scale);
protected:
  Ogre::SceneManager* sceneMgr;
  Ogre::Entity* entity;
  Ogre::SceneNode* node;
  Ogre::Vector3 velocity;
  Ogre::String name;
  Ogre::Vector3 initialForward;

  btRigidBody *body;
};

#endif // __GameObject_h__
