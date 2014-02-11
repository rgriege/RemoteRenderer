#include "FloatingNumber.h"
#include "MotionState.h"
#include <Ogre.h>
#include <sstream>

std::string FloatingNumber::nextName = "fn0";
int FloatingNumber::nextNum = 1;

FloatingNumber::FloatingNumber(CEGUI::Window* _window, CEGUI::UVector2 position, int num)
{
  speed = 0.02f;
  alpha = 1.f;
  fade = 1.f;
  parent = _window;
  //printf("screen coords (%f, %f) (%f, %f)\n", position.d_x.d_scale, position.d_x.d_offset, position.d_y.d_scale, position.d_y.d_offset);
  CEGUI::WindowManager &windMgr = CEGUI::WindowManager::getSingleton();
  window = (CEGUI::Window*)windMgr.createWindow("TaharezLook/StaticText", nextName);
  window->setHorizontalAlignment(CEGUI::HA_CENTRE);
  window->setProperty("FrameEnabled", "False");
  window->setProperty("BackgroundEnabled", "False");
  window->setProperty("HorzFormatting", "HorzCentred");
  if (num < 0) {
    num = abs(num);
    window->setProperty("TextColours", "tl:FFFF0000 tr:FFFF0000 bl:FFFF0000 br:FFFF0000");
  } else {
    window->setProperty("TextColours", "tl:FF00FF00 tr:FF00FF00 bl:FF00FF00 br:FF00FF00");
  }
  window->setPosition(position);
  window->setSize(CEGUI::UVector2(CEGUI::UDim(0.07f, 0), CEGUI::UDim(0.05f, 0)));
  std::stringstream ss;
  ss << num;
  window->setText(ss.str());
  parent->addChildWindow(window);

  ss.str(std::string());
  ss << "fn" << nextNum++;
  nextName = ss.str();
}

FloatingNumber::~FloatingNumber(void)
{
  parent->removeChildWindow(window);
  CEGUI::WindowManager::getSingleton().destroyWindow(window);
}

void FloatingNumber::update(Ogre::Real dt)
{
  if (alpha > 0.0f) {
    Ogre::Real fadeScale = fade*dt*.001f;
    if (alpha - fadeScale > 0.0f)
      alpha -= fadeScale;
    else
      alpha = 0.0f;
    //translate(Ogre::Vector3(0, speed*dt, 0));
    window->setAlpha(alpha);
  }
}

bool FloatingNumber::finished()
{
  return alpha == 0.0f;
}
