#include "menuitem_bounds_pano.h"

MenuItemBoundsPano::MenuItemBoundsPano(Display *display, const std::string &name) : MenuItemBounds(display, name,true) {
    onSave([this](MenuItemBounds&self){
        getDisplay()->getPanoData().setView(getView());
    });
}