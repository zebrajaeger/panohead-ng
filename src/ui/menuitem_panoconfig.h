#pragma once

#include <functional>

#include "menuitem_base.h"
#include "menuitem_delay.h"
#include "displayutils.h"

class MenuItemPanoConfig : public MenuItemBase {
 public:
  //------------------------------------------------------------------------------
  MenuItemPanoConfig(U8G2 *u8g2, const std::string &name)
      : MenuItemBase(u8g2, name)
  //------------------------------------------------------------------------------
  {
    add(new MenuItemBase(u8g2, ".."));
    add(new MenuItemDelay(u8g2_, "Delay after move"));
    add(new MenuItemDelay(u8g2_, "Focus time"));
    add(new MenuItemDelay(u8g2_, "Trigger time"));
    // panoConfig->add(new MenuItemBase(u8g2, "Delay between shots"));
    // panoConfig->add(new MenuItemBase(u8g2, "Shot count"));
    // panoConfig->add(new MenuItemBase(u8g2, "shots"));

    onRender([this](MenuItem &menu) { DisplayUtils::renderMenu(u8g2_, menu); });

    onButtonPushed([this](MenuItem &menu) {
      if (menu.getSelector().getIndex() == 0) {
        menu.goUp();
        return false;
      }
      return true;
    });
  }
};