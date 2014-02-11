#include "Ball.h"
#include "MotionState.h"
#include <Ogre.h>

Ball::Ball(Ogre::SceneManager* _sceneMgr, Ogre::Real _radius, Ogre::String _name) : GameObject(_name)
{
  radius = _radius;

  if (_sceneMgr)
    addToGraph(_sceneMgr);

  // set up Bullet properties
  btTransform* transform = new btTransform(btMatrix3x3::getIdentity());
  MotionState *motionState = new MotionState(node, *transform);

  btCollisionShape *shape = new btSphereShape(btScalar(10.f));
  btScalar mass(1.0f);
  btVector3 inertia;
  shape->calculateLocalInertia(mass, inertia);
  body = new btRigidBody(mass, motionState, shape, inertia);
  body->setUserPointer((void *)this);
  body->setRestitution(1.f);
  body->setFriction(0.f);
}

Ball::~Ball(void)
{
}

void Ball::addToGraph(Ogre::SceneManager* sceneMgr)
{
  // set up Ogre properties
  entity = sceneMgr->createEntity("Ball", "sphere.mesh");

  // Create Cube materials
  Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("ballMat",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("checker.png");
  entity->setMaterial(mat);

  node = sceneMgr->getRootSceneNode()->createChildSceneNode("BallNode");
  node->attachObject(entity);

  Ogre::Real cur_radius = entity->getBoundingRadius();
  node->scale(radius/cur_radius, radius/cur_radius, radius/cur_radius);
  //std::string str = Ogre::StringConverter::toString(radius);
  //TRACE(str);
}

Ogre::Real Ball::getRadius()
{
  return radius;
}
