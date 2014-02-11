#ifndef __FloatingText_h__
#define __FloatingText_h__

#include "Character.h"
#include <string>

class FloatingText
{
public:
  FloatingText(CEGUI::Window* _window, Character* _character, std::string text);
  virtual ~FloatingText(void);
  virtual void update(Ogre::Matrix4 transformation);
  bool isVisible();
  void setVisible(bool visible);
  Ogre::Vector3 getCharPosition();

private:
  CEGUI::Window* parent;
  CEGUI::Window* window;
  Character* character;
  static std::string nextName;
  static int nextNum;
};

#endif //__FloatingText_h__
