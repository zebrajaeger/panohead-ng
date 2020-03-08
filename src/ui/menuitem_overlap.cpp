#include "menuitem_overlap.h"

#include "menuitem_number.h"
#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemOverlap::MenuItemOverlap(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemBase(display, ".."));
  add((new MenuItemNumber(display, "Overlap X", "%", 100000))->onSave([this](const MenuItemNumber &self) {
    getDisplay()->getPanoData().getPicture().setOverlapWidth((double)self.getValue() / 1000.0);
  }));
  add((new MenuItemNumber(display, "Overlap Y", "%", 100000))->onSave([this](const MenuItemNumber &self) {
    getDisplay()->getPanoData().getPicture().setOverlapHeight((double)self.getValue() / 1000.0);
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