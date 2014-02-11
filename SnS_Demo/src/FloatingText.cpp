#include "FloatingText.h"
#include "MotionState.h"
#include <Ogre.h>
#include <sstream>

std::string FloatingText::nextName = "ft0";
int FloatingText::nextNum = 1;

FloatingText::FloatingText(CEGUI::Window* _window, Character* _character, std::string text)
{
  character = _character;
  parent = _window;
  //printf("screen coords (%f, %f) (%f, %f)\n", position.d_x.d_scale, position.d_x.d_offset, position.d_y.d_scale, position.d_y.d_offset);
  CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();
  window = (CEGUI::Window*)windMgr.createWindow("TaharezLook/StaticText", nextName);
  window->setHorizontalAlignment(CEGUI::HA_CENTRE);
  window->setProperty("FrameEnabled", "False");
  window->setProperty("BackgroundEnabled", "False");
  window->setProperty("HorzFormatting", "HorzCentred");
  window->setProperty("TextColours", "tl:FF0000FF tr:FF0000FF bl:FF0000FF br:FF0000FF");
  int len = text.length();
  window->setSize(CEGUI::UVector2(CEGUI::UDim(0, 20*len), CEGUI::UDim(0, 40)));
  window->setText(text);
  parent->addChildWindow(window);

  std::stringstream ss;
  ss << "ft" << nextNum++;
  nextName = ss.str();
}

FloatingText::~FloatingText(void)
{
  parent->removeChildWindow(window);
  CEGUI::WindowManager::getSingleton().destroyWindow(window);
}

void FloatingText::update(Ogre::Matrix4 transformation)
{
  if(window->isVisible()) {
    Ogre::Vector3 point = transformation * (character->getPosition() + Ogre::Vector3(0, character->getHeight(), 0));
    CEGUI::UVector2 screenPos(CEGUI::UDim((point.x / 2) + 0.f, 0),
                              CEGUI::UDim(-(point.y / 2) + 0.5f, -20));
    window->setPosition(screenPos);
  }
}

bool FloatingText::isVisible()
{
  return window->isVisible();
}

void FloatingText::setVisible(bool visible)
{
  window->setVisible(visible);
}

Ogre::Vector3 FloatingText::getCharPosition()
{
  return character->getPosition();
}