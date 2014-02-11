#include "Blob.h"
#include "MotionState.h"
#include "cd_wavefront.h"
#include <Ogre.h>

Blob::Blob(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _radius) : GameObject(_name)
{
  name = _name;
  radius = _radius;
  jumpUp = false;
  jumpDown = false;
  jumpTime = 0;

  if (_sceneMgr)
    addToGraph(_sceneMgr);

  // set up Bullet properties
  btTransform* transform = new btTransform(btMatrix3x3::getIdentity());
  MotionState *motionState = new MotionState(node, *transform);

  btCollisionShape *shape = GenerateShape(5.f);

  btScalar mass(1.f);
  btVector3 localInertia(0,0,0);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
  body = new btRigidBody(rbInfo);
  body->setLinearFactor(btVector3(1.f,0.f,1.f));
  body->forceActivationState(DISABLE_DEACTIVATION);
  body->setRestitution(1.f);
  body->setFriction(0.f);
  body->setUserPointer((void *)this);
}

void Blob::addToGraph(Ogre::SceneManager* sceneMgr)
{
  // set up Ogre properties
  entity = sceneMgr->createEntity(name, "Hemisphere.mesh");

  // Create Blob materials
  Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name+"Mat",
    Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
  Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("checker.png");
  entity->setMaterial(mat);

  node = sceneMgr->getRootSceneNode()->createChildSceneNode(name);
  node->attachObject(entity);

  Ogre::Real cur_radius = entity->getBoundingRadius();
  node->scale(radius/cur_radius, radius/cur_radius, radius/cur_radius);
  //std::string str = Ogre::StringConverter::toString(radius);
  //TRACE(str);
}

Blob::~Blob(void)
{
}

Ogre::Real Blob::getRadius()
{
  return radius;
}

void Blob::setJumpUp(bool b)
{
  jumpUp = b;
}

bool Blob::getJumpUp()
{
  return jumpUp;
}

void Blob::setJumpDown(bool b)
{
  jumpDown = b;
}

bool Blob::getJumpDown()
{
  return jumpDown;
}

void Blob::setJumpTime(Ogre::Real t)
{
  jumpTime = t;
}

Ogre::Real Blob::getJumpTime()
{
  return jumpTime;
}

void Blob::jump(float vel)
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

// See ConvexDecompositionDemo.cpp in Bullet Docs
btCollisionShape* Blob::GenerateShape(float scale)
{
  ConvexDecomposition::WavefrontObj wo;
  if (!wo.loadObj("media/models/hemisphere.obj")) {
    printf("Failed to load blob obj file\n");
  }

  btTriangleMesh* trimesh = new btTriangleMesh();

  for (int i=0;i<wo.mTriCount;i++) {
    int index0 = wo.mIndices[i*3];
    int index1 = wo.mIndices[i*3+1];
    int index2 = wo.mIndices[i*3+2];

    btVector3 vertex0(wo.mVertices[index0*3], wo.mVertices[index0*3+1],wo.mVertices[index0*3+2]);
    btVector3 vertex1(wo.mVertices[index1*3], wo.mVertices[index1*3+1],wo.mVertices[index1*3+2]);
    btVector3 vertex2(wo.mVertices[index2*3], wo.mVertices[index2*3+1],wo.mVertices[index2*3+2]);

    vertex0 *= scale;
    vertex1 *= scale;
    vertex2 *= scale;

    trimesh->addTriangle(vertex0,vertex1,vertex2);
  }

  btConvexShape* shape = new btConvexTriangleMeshShape(trimesh);

  /*printf("old numTriangles= %d\n",wo.mTriCount);
  printf("new numTrinagles= %d\n",trimesh->getNumTriangles());
  printf("old numIndices = %d\n",wo.mTriCount*3);
  printf("old numVertices = %d\n",wo.mVertexCount);*/

  return shape;
}
