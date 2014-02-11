#include "BoxChunk.h"
#include "MotionState.h"
#include <Ogre.h>
#include <sstream>
#include <stdlib.h>

Ogre::String BoxChunk::nextName = "bc0";
int BoxChunk::nextNum = 1;
Ogre::Vector3 BoxChunk::gravity = Ogre::Vector3(0, -0.02f, 0);

BoxChunk::BoxChunk(Ogre::SceneManager* _sceneMgr, Character* _character) : GameObject(nextName)
{
  height = _character->getHeight()*0.2f;
  speed = _character->getHeight()*0.03f; // ((float)(rand() % 5))/10.f; 
  direction = Ogre::Vector3(rand() % 3 - 1, 2.f, rand() % 3 -1);
  direction.normalise();

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

  translate(_character->getPosition());
  std::stringstream ss;
  ss << "bc" << nextNum++;
  nextName = ss.str();
}

void BoxChunk::addToGraph(Ogre::SceneManager* sceneMgr)
{
  // set up Ogre properties
  entity = sceneMgr->createEntity(name, "cube.mesh");

  // Create BoxChunk materials
  Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name+"Mat",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  entity->setMaterial(mat);
  entity->setMaterial(mat);
  entity->setCastShadows(false);

  node = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
  node->attachObject(entity);

  Ogre::Real cur_height = entity->getBoundingBox().getSize().y;
  node->scale(height/cur_height, height/cur_height, height/cur_height);
  node->rotate(Ogre::Quaternion(((float)(rand() % 10))/10.f, ((float)(rand() % 10))/10.f,
                                ((float)(rand() % 10))/10.f, ((float)(rand() % 10))/10.f));

  Ogre::ColourValue c(1, 0, 0, 1);
  mat->setAmbient(c);
  mat->setDiffuse(c);
  mat->setSpecular(c);
}

BoxChunk::~BoxChunk(void)
{
  node->detachObject(entity);
  sceneMgr->destroyEntity(entity);
  sceneMgr->destroySceneNode(node);
}

void BoxChunk::update(Ogre::Real dt)
{
  if (getPosition().y > -height) {
    translate(direction * speed);
    direction += gravity;
  }
}

bool BoxChunk::finished()
{
  return getPosition().y <= -height;
}
