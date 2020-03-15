#include "menuitem_main.h"

#include "menuitem_qr.h"
#include "menuitem_bounds.h"
#include "menuitem_level.h"
#include "menuitem_joystick.h"
#include "menuitem_panoconfig.h"
#include "menuitem_power.h"
#include "menuitem_overlap.h"
#include "displayutils.h"

#include "distributor.h"

//------------------------------------------------------------------------------
MenuItemMain::MenuItemMain(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemPanoConfig(display, "Pan cfg"));
  add((new MenuItemBounds(display, "Pan bnd", true))->onSave([this](MenuItemBounds &self) {
    Distributor::getInstance().getView().set(self.getView());
  }));
  add((new MenuItemBounds(display, "Pic bnd", false))->onSave([this, display](MenuItemBounds &self) {
    Picture& picture = Distributor::getInstance().getPicture().get();
    picture.setWidth(self.getView().calculateWidth());
    picture.setHeight(self.getView().calculateHeight());
  }));
  add(new MenuItemOverlap(display, "Pic ovl"));
  add(new MenuItemBase(display, "Take pano"))->disable();
  add(new MenuItemPower(display, "Power"));
  add(new MenuItemQR(display, "QR"));
  add(new MenuItemLevel(display, "Leveling"));
  add(new MenuItemJoystick(display, "Joystick"));

  onRender([this](MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();
    DisplayUtils::renderMenu(u8g2, menu);
  });
}