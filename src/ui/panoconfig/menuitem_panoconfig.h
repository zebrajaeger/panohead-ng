#pragma once

#include <functional>

#include "ui/menuitem_base.h"

class MenuItemPanoConfig : public MenuItemBase {
 public:
  MenuItemPanoConfig(Display *display, const std::string &name);
};