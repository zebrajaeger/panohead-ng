#include "menuitem_main.h"

#include "menuitem_delay.h"
#include "menuitem_qr.h"
#include "menuitem_bounds_pano.h"
#include "menuitem_bounds_picture.h"
#include "menuitem_level.h"
#include "menuitem_panoconfig.h"
#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemMain::MenuItemMain(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemPanoConfig(display, "Pano cfg"));
  add(new MenuItemBoundsPano(display, "Pano bnd"));
  add(new MenuItemBoundsPicture(display, "Pic bnd"));
  add(new MenuItemLevel(display, "Leveling"));
  add(new MenuItemQR(display, "QR"));
  add(new MenuItemBase(display, "Take pano"))->disable();

  onRender([this](MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();
    DisplayUtils::renderMenu(u8g2, menu);
  });
}