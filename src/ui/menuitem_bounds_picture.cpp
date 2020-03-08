#include "menuitem_bounds_picture.h"

#include "pano/picture.h"

MenuItemBoundsPicture::MenuItemBoundsPicture(Display *display, const std::string &name) : MenuItemBounds(display, name, false) {
  onSave([this](MenuItemBounds &self) {
    Picture &picture = getDisplay()->getPanoData().getPicture();
    const View &view = getView();
    picture.setWidth(view.calculateWidth());
    picture.setHeight(view.calculateHeight());
  });
}