#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemTakePano : public MenuItemBase {
 public:
  MenuItemTakePano(Display *display, const std::string &name);

 private:
  void render(MenuItem &menu);
};