#pragma once

#include <Arduino.h>

#include "display.h"

#include "menuitem_bounds.h"

class MenuItemBoundsPano : public MenuItemBounds {
 public:
  MenuItemBoundsPano(Display *display, const std::string &name);
};