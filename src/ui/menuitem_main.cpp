#include "menuitem_main.h"

#include "./bounds/menuitem_bounds.h"
#include "./bounds/menuitem_bounds_pano.h"
#include "./bounds/menuitem_bounds_picture.h"
#include "./joystick/menuitem_joystick.h"
#include "./level/menuitem_level.h"
#include "./overlap/menuitem_overlap.h"
#include "./panoconfig/menuitem_panoconfig.h"
#include "./power/menuitem_power.h"
#include "./qr/menuitem_qr.h"
#include "./shot/menuitem_shots.h"
#include "./takepano/menuitem_takepano.h"
#include "displayutils.h"
#include "distributor.h"

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