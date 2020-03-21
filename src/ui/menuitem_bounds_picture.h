#pragma once

#include "distributor.h"
#include "menuitem_bounds.h"
#include "pano/picture.h"

class MenuItemBoundsPicture : public MenuItemBounds {
 public:
  MenuItemBoundsPicture(Display *display, const std::string &name) : MenuItemBounds(display, name, false) {
    onSave([this, display](MenuItemBounds &self) {
      Picture &picture = Distributor::getInstance().getPicture().get();
      picture.setWidth(self.getView().calculateWidth());
      picture.setHeight(self.getView().calculateHeight());
      Distributor::getInstance().getPicture().propagateChange();
    });
  }
};