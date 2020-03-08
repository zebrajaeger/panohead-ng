#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemBoot : public MenuItemBase {
 public:
  MenuItemBoot(Display *display, const std::string &name);
  void render(MenuItem &menu);
};