#pragma once

#include "distributor.h"
#include "menuitem_bounds.h"

class MenuItemBoundsPano : public MenuItemBounds {
 public:
  MenuItemBoundsPano(Display *display, const std::string &name) : MenuItemBounds(display, name, true) {
    onSave([this](MenuItemBounds &self) { Distributor::getInstance().getView().set(self.getView()); });
  }
};