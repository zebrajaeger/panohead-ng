#pragma once

#include <Arduino.h>

#include "display.h"

#include "menuitem_bounds.h"

class MenuItemBoundsPicture : public MenuItemBounds {
 public:
  MenuItemBoundsPicture(Display *display, const std::string &name);
};