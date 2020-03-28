#include "menuitem_main.h"

#include "displayutils.h"
#include "distributor.h"
#include "menuitem_bounds.h"
#include "menuitem_bounds_pano.h"
#include "menuitem_bounds_picture.h"
#include "menuitem_joystick.h"
#include "menuitem_level.h"
#include "menuitem_overlap.h"
#include "menuitem_panoconfig.h"
#include "menuitem_power.h"
#include "menuitem_qr.h"
#include "menuitem_shots.h"
#include "menuitem_takepano.h"

//------------------------------------------------------------------------------
MenuItemMain::MenuItemMain(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  add(new MenuItemPanoConfig(display, "Move cfg"));
  add(new MenuItemBoundsPano(display, "Pano rect"));
  add(new MenuItemBoundsPicture(display, "Pic rect"));
  add(new MenuItemOverlap(display, "Pic ovl"));
  add(new MenuItemShots(display, "Shots"));
  add(new MenuItemTakePano(display, "Take pano"));
  add(new MenuItemQR(display, "QR"));
  add(new MenuItemPower(display, "Power"));
  add(new MenuItemLevel(display, "Leveling"));
  add(new MenuItemJoystick(display, "Joystick"));

  onRender([this](MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();
    DisplayUtils::renderMenu(u8g2, menu);
  });
}