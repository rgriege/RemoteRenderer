#include "Box.h"
#include "OgreMeshManager.h"
#include "MotionState.h"

Box::Box(Ogre::SceneManager* _sceneMgr, Ogre::Vector3 _halfExtents, Ogre::String _name) : GameObject(_name)
{
  halfExtents = _halfExtents;

  if (_sceneMgr)
    addToGraph(_sceneMgr);

  // Set up Bullet properties
  MotionState *motionState = new MotionState(node);
  btCollisionShape* shape = new btBoxShape(btVector3(halfExtents.x/2, halfExtents.y/2, halfExtents.z/2));
  btScalar mass(0.f);
  btVector3 localInertia(0,0,0);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
  body = new btRigidBody(rbInfo);
  body->setLinearFactor(btVector3(0.15f,0,0.15f));
  body->forceActivationState(DISABLE_DEACTIVATION);
  body->setRestitution(1.f);
  body->setFriction(0.f);
  body->setUserPointer((void *)this);

  body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

void Box::addToGraph(Ogre::SceneManager* sceneMgr)
{
  // Set up Ogre properties
  entity = sceneMgr->createEntity("Box", "cube.mesh");

  node = sceneMgr->getRootSceneNode()->createChildSceneNode("BoxNode");
  node->attachObject(entity);

  Ogre::AxisAlignedBox box = entity->getBoundingBox();
  Ogre::Vector3 size = box.getSize();
  Ogre::Vector3 newSize = halfExtents;
  node->scale(1/size);
  node->scale(newSize);
}

Box::~Box(void)
{
}
