#include "menuitem_main.h"

#include "menuitem_delay.h"
#include "menuitem_qr.h"
#include "menuitem_bounds.h"
#include "menuitem_level.h"
#include "menuitem_panoconfig.h"
#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemMain::MenuItemMain(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemPanoConfig(display, "Pano config"));
  add(new MenuItemBounds(display, "Set bounds"));
  add(new MenuItemBase(display, "Take pano"))->disable();
  //   add(new MenuItemLevel(display, "Leveling"));
  add(new MenuItemQR(display, "QR"));

  onRender([this](MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();
    DisplayUtils::renderMenu(u8g2, menu);
  });
}