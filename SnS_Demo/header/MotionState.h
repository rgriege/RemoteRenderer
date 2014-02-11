#ifndef __MotionState_h__
#define __MotionState_h__

#include "LinearMath/btMotionState.h"
#include "stdafx.h"

struct MotionState : public btMotionState
{
  Ogre::SceneNode* node;
  btTransform m_pos;

  MotionState(Ogre::SceneNode* _node, const btTransform& initialpos = btTransform::getIdentity())
    : m_pos(initialpos), node(_node)
  {
  }
  virtual void getWorldTransform(btTransform& worldTrans) const
  {
    worldTrans = m_pos;
  }
  
  virtual void setWorldTransform(const btTransform& worldTrans)
  {
    m_pos = worldTrans;
    if (node) {
      const btVector3 pos = m_pos.getOrigin();
      const btQuaternion rot = m_pos.getRotation();
      node->setPosition(pos.x(), pos.y(), pos.z());
      node->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
    }
  }
};

#endif // __MotionState_h__
