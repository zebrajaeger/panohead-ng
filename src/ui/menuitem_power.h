#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemPower : public MenuItemBase {
 public:
  MenuItemPower(Display *display, const std::string &name);

 private:
  void render(MenuItem &menu);
};