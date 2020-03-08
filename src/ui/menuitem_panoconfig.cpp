#include "menuitem_panoconfig.h"

#include "menuitem_delay.h"
#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemPanoConfig::MenuItemPanoConfig(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemBase(display, ".."));
  add((new MenuItemDelay(display, "Delay after move"))->onSave([this](int32_t timeMs) {
    getDisplay()->getPanoData().setDelayAfterMove(timeMs);
  }));
  add((new MenuItemDelay(display, "Focus time"))->onSave([this](int32_t timeMs) { getDisplay()->getPanoData().setFocusTime(timeMs); }));
  add((new MenuItemDelay(display, "Trigger time"))->onSave([this](int32_t timeMs) { getDisplay()->getPanoData().setTriggerTime(timeMs); }));
  // panoConfig->add(new MenuItemBase(u8g2, "Delay between shots"));
  // panoConfig->add(new MenuItemBase(u8g2, "Shot count"));
  // panoConfig->add(new MenuItemBase(u8g2, "shots"));

  onRender([this](MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();
    DisplayUtils::renderMenu(u8g2, menu);
  });

  onButtonPushed([this](MenuItem &menu) {
    if (menu.getSelector().getIndex() == 0) {
      menu.goUp();
      return false;
    }
    return true;
  });
}