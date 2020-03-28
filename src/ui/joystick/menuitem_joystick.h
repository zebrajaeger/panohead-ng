#pragma once

#include <functional>

#include "ui/menuitem_base.h"

class MenuItemJoystick : public MenuItemBase {
 public:
  MenuItemJoystick(Display *display, const std::string &name);

 private:
  void render(MenuItem &menu);
};