#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemOverlap : public MenuItemBase {
 public:
  MenuItemOverlap(Display *display, const std::string &name);
};