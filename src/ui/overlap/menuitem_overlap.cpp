#include "menuitem_overlap.h"

#include "distributor.h"
#include "ui/displayutils.h"
#include "ui/menuitem_number.h"

//------------------------------------------------------------------------------
MenuItemOverlap::MenuItemOverlap(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  // up
  add(new MenuItemBase(display, ".."));

  // overlap X
  MenuItemNumber *overlapX = new MenuItemNumber(display, "Overlap X", "%", 100000);
  overlapX->onEnter([overlapX](MenuItem &self) { overlapX->setValue((*Distributor::i.getPicture()).getOverlapWidth() * 1000.0); });
  Distributor::i.getPicture() += [overlapX](const Value<Picture> &picture) { overlapX->setValue((*picture).getOverlapWidth() * 1000); };
  overlapX->onSave([this](const MenuItemNumber &self) {
    Value<Picture> &picture = Distributor::i.getPicture();
    (*picture).setOverlapWidth((double)self.getValue() / 1000.0);
    picture.propagateChange();
  });
  add(overlapX);

  // overlap Y
  MenuItemNumber *overlapY = new MenuItemNumber(display, "Overlap Y", "%", 100000);
  overlapY->onEnter([overlapY](MenuItem &self) { overlapY->setValue((*Distributor::i.getPicture()).getOverlapHeight() * 1000.0); });
  Distributor::i.getPicture() += [overlapY](const Value<Picture> &picture) { overlapY->setValue((*picture).getOverlapHeight() * 1000); };
  overlapY->onSave([this](const MenuItemNumber &self) {
    Value<Picture> &picture = Distributor::i.getPicture();
    (*picture).setOverlapHeight((double)self.getValue() / 1000.0);
    picture.propagateChange();
  });
  add(overlapY);

  // self
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