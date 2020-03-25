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

  add((new MenuItemNumber(display, "Del aft mov", "s", 10000))
          ->onSave([this](const MenuItemNumber &self) {
            Distributor::i.getMovementTiming().get().setDelayAfterMoveMs(self.getValue());
            Distributor::i.getMovementTiming().propagateChange();
          })
          ->onEnter([](MenuItem &self) {
            MenuItemNumber &menuItemNumber = (MenuItemNumber &)self;
            Distributor::i.getMovementTiming().get(
                [&menuItemNumber](const MovementTiming &value) { 
                  menuItemNumber.setValue(value.getDelayAfterMoveMs()); });
          }));

  add((new MenuItemNumber(display, "Del btw shts", "s", 10000))
          ->onSave([this](const MenuItemNumber &self) {
            Distributor::i.getMovementTiming().get().setDelayBetweenShotsMs(self.getValue());
            Distributor::i.getMovementTiming().propagateChange();
          })
          ->onEnter([](MenuItem &self) {
            MenuItemNumber &menuItemNumber = (MenuItemNumber &)self;
            Distributor::i.getMovementTiming().get(
                [&menuItemNumber](const MovementTiming &value) { menuItemNumber.setValue(value.getDelayBetweenShotsMs()); });
          }));

  add((new MenuItemNumber(display, "Del lst sht", "s", 10000))
          ->onSave([this](const MenuItemNumber &self) {
            Distributor::i.getMovementTiming().get().setDelayAfterLastShotMs(self.getValue());
            Distributor::i.getMovementTiming().propagateChange();
          })
          ->onEnter([](MenuItem &self) {
            MenuItemNumber &menuItemNumber = (MenuItemNumber &)self;
            Distributor::i.getMovementTiming().get(
                [&menuItemNumber](const MovementTiming &value) { menuItemNumber.setValue(value.getDelayAfterLastShotMs()); });
          }));

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