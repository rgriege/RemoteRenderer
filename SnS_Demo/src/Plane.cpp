#include "Plane.h"
#include "OgreMeshManager.h"
#include <Ogre.h>
#include "MotionState.h"

Plane::Plane(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Vector3 _normal, Ogre::Vector3 _upVector, Ogre::Real _coord) : GameObject(_name)
{
  normal = _normal;
  upVector = _upVector;
  coord = _coord;

  if (_sceneMgr)
    addToGraph(_sceneMgr);

  // Set up Bullet properties
  MotionState *motionState = new MotionState(node);
  btCollisionShape *shape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), coord);
  btScalar mass(0.f);
  btVector3 localInertia(0,0,0);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
  body = new btRigidBody(rbInfo);
  body->setRestitution(1.f);
  if (normal.y == 1)
    body->setRestitution(0.f);
  body->setFriction(0.f);
  body->setUserPointer((void *)this);
}

void Plane::addToGraph(Ogre::SceneManager* sceneMgr)
{
  // Set up Ogre properties
  
  // Create Cube materials
  Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name+"Mat",
  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("grass_1024.jpg");

  // Create plane
  Ogre::Plane plane(normal, coord);
  Ogre::MeshManager::getSingleton().createPlane(name,
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 10000, 10000, 5, 5, true, 1, 1, 1, upVector);

  // Create Entity & Node
  entity = sceneMgr->createEntity(name, name);
  entity->setMaterial(mat);
  entity->setCastShadows(false);


  node = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
  node->attachObject(entity);
}

Plane::~Plane(void)
{
}

Ogre::Vector3 Plane::getNormal(void)
{
  return normal;
}
