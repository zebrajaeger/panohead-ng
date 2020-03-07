#pragma once

#include "menuitem_base.h"
#include "menuitem_delay.h"
#include "menuitem_bounds.h"
#include "menuitem_level.h"
#include "menuitem_panoconfig.h"
#include "displayutils.h"

class MenuItemMain : public MenuItemBase {
 public:
  //------------------------------------------------------------------------------
  MenuItemMain(U8G2 *u8g2, const std::string &name)
      : MenuItemBase(u8g2, name)
  //------------------------------------------------------------------------------
  {
    add(new MenuItemPanoConfig(u8g2_, "Pano config"));
    add(new MenuItemBounds(u8g2_, "Set bounds"));
    add(new MenuItemBase(u8g2, "Take pano"))->disable();
    add(new MenuItemLevel(u8g2_, "Leveling"));

    onRender([this](MenuItem &menu) { DisplayUtils::renderMenu(u8g2_, menu); });
  }
};