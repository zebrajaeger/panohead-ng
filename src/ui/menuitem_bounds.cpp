#include "menuitem_bounds.h"

#include "pano/panoutils.h"

#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemBounds::MenuItemBounds(Display *display, const std::string &name)
    : MenuItemBase(display, name),
      posRevX_(0.0),
      posRevY_(0.0),
      view_(1.234, 12.345, 123.456, 0.0),
      saveCallback_()
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemBounds::render, this, _1));
  onButtonPushed(bind(&MenuItemBounds::pushButtonSetBounds, this, _1));

  add(new MenuItemBase(display, "TogglePartial"));
  add(new MenuItemBase(display, "Top"));
  add(new MenuItemBase(display, "Top Right"));
  add(new MenuItemBase(display, "Right"));
  add(new MenuItemBase(display, "Bottom Right"));
  add(new MenuItemBase(display, "Bottom"));
  add(new MenuItemBase(display, "Bottom Left"));
  add(new MenuItemBase(display, "Left"));
  add(new MenuItemBase(display, "Top Left"));
  add(new MenuItemBase(display, "Ok"));
  add(new MenuItemBase(display, "Cancel"));
}

//------------------------------------------------------------------------------
void MenuItemBounds::onSave(SaveCallback_t saveCallback)
//------------------------------------------------------------------------------
{
  saveCallback_ = saveCallback;
}

//------------------------------------------------------------------------------
void MenuItemBounds::setPositionX(double revX)
//------------------------------------------------------------------------------
{
  posRevX_ = revX;
  requireRepaint();
}

//------------------------------------------------------------------------------
void MenuItemBounds::setPositionY(double revY)
//------------------------------------------------------------------------------
{
  posRevY_ = revY;
  requireRepaint();
}

void MenuItemBounds::setPartial(bool isPartial) {
  bool enabled = !isPartial;
  (*this)["Top Right"]->setEnabled(enabled);
  (*this)["Right"]->setEnabled(enabled);
  (*this)["Bottom Right"]->setEnabled(enabled);
  (*this)["Bottom Left"]->setEnabled(enabled);
  (*this)["Left"]->setEnabled(enabled);
  (*this)["Top Left"]->setEnabled(enabled);
}

//------------------------------------------------------------------------------
void MenuItemBounds::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  switch ((Index)menu.getSelector().getIndex()) {
    case Index::PARTIAL:
      render_(true, false, false, false, false, false, false);
      break;
    case Index::TOP:
      render_(false, true, false, false, false, false, false);
      break;
    case Index::TOP_RIGHT:
      render_(false, true, true, false, false, false, false);
      break;
    case Index::RIGHT:
      render_(false, false, true, false, false, false, false);
      break;
    case Index::BOTTOM_RIGHT:
      render_(false, false, true, true, false, false, false);
      break;
    case Index::BOTTOM:
      render_(false, false, false, true, false, false, false);
      break;
    case Index::BOTTOM_LEFT:
      render_(false, false, false, true, true, false, false);
      break;
    case Index::LEFT:
      render_(false, false, false, false, true, false, false);
      break;
    case Index::TOP_LEFT:
      render_(false, true, false, false, true, false, false);
      break;
    case Index::OK:
      render_(false, false, false, false, false, true, false);
      break;
    case Index::CANCEL:
      render_(false, false, false, false, false, false, true);
      break;
  }
}

//------------------------------------------------------------------------------
void MenuItemBounds::render_(bool togglePartial, bool top, bool right, bool bottom, bool left, bool ok, bool cancel)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();

  u8g2->clearBuffer();

  // partial / full pano
  DisplayUtils::drawSymbolAt(u8g2, 22, 24 + 16, togglePartial, view_.isPartial() ? 0x8c : 0xf6);

  // top
  if (top) {
    u8g2->drawBox(10, 0, 44, 10);
  } else {
    u8g2->drawFrame(10, 0, 44, 10);
  }
  // bottom
  if (bottom) {
    u8g2->drawBox(10, 53, 44, 10);
  } else {
    u8g2->drawFrame(10, 53, 44, 10);
  }

  if (!view_.isPartial()) {
    // left
    if (left) {
      u8g2->drawBox(0, 10, 10, 44);
    } else {
      u8g2->drawFrame(0, 10, 10, 44);
    }
    // right
    if (right) {
      u8g2->drawBox(54, 10, 10, 44);
    } else {
      u8g2->drawFrame(54, 10, 10, 44);
    }
  }

  // pos:
  u8g2->setFont(u8g2_font_timR10_tf);
  u8g2->drawStr(71, 8, "x:");
  DisplayUtils::drawAngleAt(u8g2, 87, -2, false, false, PanoUtils::revToDeg(posRevX_));
  u8g2->drawStr(71, 22, "y:");
  DisplayUtils::drawAngleAt(u8g2, 87, 12, false, false, PanoUtils::revToDeg(posRevY_));

  // ok
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8, 60, ok, 0x73);

  // cancel
  u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8 + 24, 60, cancel, 0x11b);

  u8g2->sendBuffer();
}

//------------------------------------------------------------------------------
bool MenuItemBounds::pushButtonSetBounds(MenuItem &menu)
//------------------------------------------------------------------------------
{
  switch (menu.getSelector().getIndex()) {
    case Index::PARTIAL:
      view_.setPartial(!view_.isPartial());
      setPartial(view_.isPartial());
      menu.requireRepaint();
      break;
    case Index::OK:
      if (saveCallback_) {
        saveCallback_(view_);
        getDisplay()->getPanoData().setView(view_);
      }
      menu.goUp();
      break;
    case Index::CANCEL:
      menu.goUp();
      break;
    default:
      break;
  }
  return false;
}
