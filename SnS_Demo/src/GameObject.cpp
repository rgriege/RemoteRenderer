#include "GameObject.h"
#include "cd_wavefront.h"
#include <OgreSubMesh.h>

GameObject::GameObject(Ogre::String _name)
{
  name = _name;
  initialForward = Ogre::Vector3::UNIT_Z;
}

GameObject::~GameObject(void)
{
}

Ogre::String GameObject::getName(void)
{
	return name;
}

void GameObject::translate(const Ogre::Vector3 displacement)
{
	if (node)
		node->translate(displacement);
	body->translate(btVector3(displacement.x, displacement.y, displacement.z));
}

void GameObject::showBoundingBox(bool show)
{
  if (node)
    node->showBoundingBox(show);
  else
    printf("showBoundingBox: Cannot show bounding box. Node is NULL\n");
}

Ogre::Vector3 GameObject::getPosition()
{
  if (node)
    return node->getPosition();
  else {
    btVector3 pos = body->getCenterOfMassPosition();
    return Ogre::Vector3(pos.x(), pos.y(), pos.z());
  }
}

Ogre::Radian GameObject::getYaw()
{
  if (node)
    return node->getOrientation().getYaw();
  return Ogre::Radian(0);
}

void GameObject::yaw(const Ogre::Radian radians)
{
    if (node)
        node->yaw(radians);
    btTransform t = body->getCenterOfMassTransform();
    t.setRotation(btQuaternion(0, 1, 0, radians.valueRadians()));
    body->setCenterOfMassTransform(t);
}

void GameObject::setYaw(const Ogre::Radian radians)
{
  if (node) {
    node->setOrientation(node->getInitialOrientation());
  }
  yaw(radians);
}


Ogre::Vector3 GameObject::getDirection()
{
  if (node) {
    Ogre::Quaternion quat = node->getOrientation();
    return quat * initialForward;
  }
  return Ogre::Vector3::UNIT_Z;
}

void GameObject::setDirection(const Ogre::Vector3 direction)
{
  Ogre::Radian r = initialForward.angleBetween(direction);
  Ogre::Vector3 initialRight = initialForward.crossProduct(Ogre::Vector3::UNIT_Y);
  if (initialRight.dotProduct(direction) > 0.f)
    r *= -1;
  setYaw(r);
}

void GameObject::setPosition(const Ogre::Vector3 position)
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
}

Ogre::Vector3 GameObject::getVelocity()
{
  return velocity;
}

void GameObject::setVelocity(Ogre::Vector3 _velocity)
{
  velocity = _velocity;
}

btRigidBody* GameObject::getRigidBody(void)
{
  return body;
}

// See ConvexDecompositionDemo.cpp in Bullet Docs
btCollisionShape* GameObject::GenerateShape(std::string file, float scale)
{
  ConvexDecomposition::WavefrontObj wo;
  if (!wo.loadObj (strcat ("media/models/", file.c_str ()))) {
    printf("Failed to load Player obj file\n");
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
