#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemPanoConfig : public MenuItemBase {
 public:
  MenuItemPanoConfig(Display *display, const std::string &name);
};