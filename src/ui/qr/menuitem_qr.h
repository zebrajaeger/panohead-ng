#pragma once

#include <functional>

#include "ui/menuitem_base.h"

class MenuItemQR : public MenuItemBase {
 public:
  MenuItemQR(Display *display, const std::string &name);

 private:
  void render(MenuItem &menu);
};