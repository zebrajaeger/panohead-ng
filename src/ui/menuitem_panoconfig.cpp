#include "menuitem_panoconfig.h"

#include "menuitem_number.h"
#include "displayutils.h"

#include "distributor.h"

//------------------------------------------------------------------------------
MenuItemPanoConfig::MenuItemPanoConfig(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemBase(display, ".."));
  add((new MenuItemNumber(display, "Delay after move", "s", 10000))->onSave([this](const MenuItemNumber &self) {
    Distributor::getInstance().getDelayAfterMove().set(self.getValue());
  }));
  add((new MenuItemNumber(display, "Focus time", "s", 10000))->onSave([this](const MenuItemNumber &self) {
    Distributor::getInstance().getFocusTime().set(self.getValue());
  }));
  add((new MenuItemNumber(display, "Trigger time", "s", 10000))->onSave([this](const MenuItemNumber &self) {
    Distributor::getInstance().getTriggerTime().set(self.getValue());
  }));
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