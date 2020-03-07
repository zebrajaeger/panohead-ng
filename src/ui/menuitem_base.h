#pragma once

#include "menu/menuitem.h"
#include <functional>

#include <U8g2lib.h>

#include "displayutils.h"

class MenuItemBase : public MenuItem {
 public:
  MenuItemBase(U8G2 *u8g2, const std::string &name) : MenuItem(name), u8g2_(u8g2) {}

  virtual void setPositionX(double revX) {}
  virtual void setPositionY(double revY) {}
  virtual void setLeveling(float x, float y){};

  U8G2 *u8g2_;
};