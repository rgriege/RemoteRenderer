#ifndef __FloatingNumber_h__
#define __FloatingNumber_h__

#include "stdafx.h"
#include <string>

class FloatingNumber
{
public:
  FloatingNumber(CEGUI::Window* _window, CEGUI::UVector2 position, int num);
  virtual ~FloatingNumber(void);
  virtual void update(Ogre::Real dt);
  bool finished();

private:
  CEGUI::Window* parent;
  CEGUI::Window* window;
  Ogre::Real speed;
  Ogre::Real alpha;
  Ogre::Real fade;
  static std::string nextName;
  static int nextNum;
};

#endif //__FloatingNumber_h__
