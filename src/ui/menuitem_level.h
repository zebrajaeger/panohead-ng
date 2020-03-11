#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemLevel : public MenuItemBase {
 public:
  MenuItemLevel(Display *display, const std::string &name);

 private:
  void render(MenuItem &menu);
};