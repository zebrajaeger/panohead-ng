#include "menuitem_panoconfig.h"

#include "displayutils.h"
#include "distributor.h"
#include "menuitem_number.h"

//------------------------------------------------------------------------------
MenuItemPanoConfig::MenuItemPanoConfig(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemBase(display, ".."));
  add((new MenuItemNumber(display, "Delay after move", "s", 10000))
          ->onSave([this](const MenuItemNumber &self) {
            int32_t v = self.getValue();
            LOG.d("MEEEEEEEEEP %d", v);
             Distributor::getInstance().getDelayAfterMove().set(v); })
          ->onEnter([](MenuItem &self) {
            MenuItemNumber &menuItemNumber = (MenuItemNumber &)self;
            Distributor::i.getDelayAfterMove().get([&menuItemNumber](int32_t value) { menuItemNumber.setValue(value); });
          }));
  add((new MenuItemNumber(display, "Focus time", "s", 10000))
          ->onSave([this](const MenuItemNumber &self) { Distributor::getInstance().getFocusTime().set(self.getValue()); })
          ->onEnter([](MenuItem &self) {
            MenuItemNumber &menuItemNumber = (MenuItemNumber &)self;
            Distributor::i.getFocusTime().get([&menuItemNumber](int32_t value) { menuItemNumber.setValue(value); });
          }));
  add((new MenuItemNumber(display, "Trigger time", "s", 10000))
          ->onSave([this](const MenuItemNumber &self) { Distributor::getInstance().getTriggerTime().set(self.getValue()); })
          ->onEnter([](MenuItem &self) {
            MenuItemNumber &menuItemNumber = (MenuItemNumber &)self;
            Distributor::i.getTriggerTime().get([&menuItemNumber](int32_t value) { menuItemNumber.setValue(value); });
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