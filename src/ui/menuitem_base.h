#pragma once

#include "menu/menuitem.h"
#include <functional>

#include <U8g2lib.h>

#include "display.h"
#include "displayutils.h"

class MenuItemBase : public MenuItem {
 public:
  MenuItemBase(Display *display, const std::string &name) : MenuItem(name), display_(display) {}

  virtual void setPositionX(double revX) {}
  virtual void setPositionY(double revY) {}
  virtual void setLeveling(float x, float y){};

  U8G2 *getU8g2();
  Display *getDisplay();

 private:
  Display *display_;
};