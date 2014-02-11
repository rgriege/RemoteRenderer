#include "Robot.h"
#include <Ogre.h>
#include <stdlib.h>
#include <time.h> 

Robot::Robot(Ogre::String _name, Ogre::SceneManager* _sceneMgr, Ogre::Real _height) : Enemy(_name, _sceneMgr, _height, "robot.mesh")
{
  printf("Robot created\n");

  animationState = entity->getAnimationState("Idle");
  animationState->setLoop(true);
  animationState->setEnabled(true);

  speed = 0.01f;

  initialForward = Ogre::Vector3::UNIT_X;

  printf("initial rot %f\n", getYaw().valueDegrees());
}

Robot::~Robot(void)
{
}

void Robot::update(Ogre::Real dt)
{
  switch(state) {
    case STAND:
      animationState->addTime(0.0001f*speed*dt);
      if (foundTarget())
        setState(SEEK);
      else if (rand() % 400 == 1)
        setState(WANDER);
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
        animationState->addTime(0.0001f*dt);
      }
      if (foundTarget())
        setState(SEEK);
      break;
    }
    case SEEK:
    {
      Ogre::Vector3 targetDiff = target->getPosition() - this->getPosition();
      if (targetDiff.length() > 20) {
        setState(WANDER);
      } else if (targetDiff.length() > 5) {
        setVelocity(Ogre::Vector3::ZERO);
      }
      break;
    }
  }
}

void Robot::setState(int newState)
{
  if (newState == WANDER) {
    targetLocation = getNewTargetLocation();
    faceTargetLocation();
    animationState = entity->getAnimationState("Walk");
    animationState->setLoop(true);
    animationState->setEnabled(true);
  } else if (newState == STAND) {
    animationState = entity->getAnimationState("Idle");
    animationState->setLoop(true);
    animationState->setEnabled(true);
  }
  state = newState;
}
