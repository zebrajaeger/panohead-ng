#include "menuitem_bounds.h"

#include "displayutils.h"
#include "distributor.h"
#include "pano/panoutils.h"

//------------------------------------------------------------------------------
MenuItemBounds::MenuItemBounds(Display *display, const std::string &name, bool showPartialOption)
    : MenuItemBase(display, name),
      LOG(LoggerFactory::getLogger("MenuItemBounds")),
      showPartialOption_(showPartialOption),
      view_(),
      saveCallback_()
//------------------------------------------------------------------------------
{
  if (!showPartialOption) {
    view_.setPartial(false);
  }

  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemBounds::render, this, _1));
  onButtonPushed(bind(&MenuItemBounds::pushButtonSetBounds, this, _1));
  onEnter([this](MenuItem &self) { LOG.view_.clear(); });

  Distributor::getInstance().getPosition().addListener([this](const Position &value) { requireRepaint(); });

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
void MenuItemBounds::setPartialItemsVisible(bool isPartial)
//------------------------------------------------------------------------------
{
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

  if (showPartialOption_) {
    // partial / full pano
    DisplayUtils::drawSymbolAt(u8g2, 22, 24 + 16, togglePartial, view_.isPartial() ? 0x8c : 0xf6);
  }

  // top
  if (top) {
    u8g2->drawBox(10, 0, 44, 10);
  } else {
    u8g2->drawFrame(10, 0, 44, 10);
    if (view_.getY1()) {
      u8g2->drawFrame(12, 2, 40, 6);
    }
  }
  // bottom
  if (bottom) {
    u8g2->drawBox(10, 53, 44, 10);
  } else {
    u8g2->drawFrame(10, 53, 44, 10);
    if (view_.getY2()) {
      u8g2->drawFrame(12, 55, 40, 6);
    }
  }

  if (!view_.isPartial()) {
    // left
    if (left) {
      u8g2->drawBox(0, 10, 10, 44);
    } else {
      u8g2->drawFrame(0, 10, 10, 44);
      if (view_.getX1()) {
        u8g2->drawFrame(2, 12, 6, 40);
      }
    }
    // right
    if (right) {
      u8g2->drawBox(54, 10, 10, 44);
    } else {
      u8g2->drawFrame(54, 10, 10, 44);
      if (view_.getX2()) {
        u8g2->drawFrame(56, 12, 6, 40);
      }
    }
  }

  // pos:
  Position &pos = Distributor::getInstance().getPosition().get();
  u8g2->setFont(u8g2_font_timR10_tf);
  u8g2->drawStr(71, 8, "x:");
  DisplayUtils::drawAngleAt(u8g2, 87, -2, false, false, PanoUtils::revToDeg(pos.getX()));
  u8g2->drawStr(71, 22, "y:");
  DisplayUtils::drawAngleAt(u8g2, 87, 12, false, false, PanoUtils::revToDeg(pos.getY()));

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
  uint8_t index = menu.getSelector().getIndex();
  bool top = index == Index::TOP || index == Index::TOP_LEFT || index == Index::TOP_RIGHT;
  bool bottom = index == Index::BOTTOM || index == Index::BOTTOM_LEFT || index == Index::BOTTOM_RIGHT;
  bool left = index == Index::LEFT || index == Index::TOP_LEFT || index == Index::BOTTOM_LEFT;
  bool right = index == Index::RIGHT || index == Index::TOP_RIGHT || index == Index::BOTTOM_RIGHT;

  if (top || bottom || left || right) {
    Position &pos = Distributor::getInstance().getPosition().get();
    if (top) {
      LOG.i("Set TOP to %f", pos.getY());
      view_.setY1(pos.getY());
      requireRepaint();
    }
    if (bottom) {
      LOG.i("Set BOTTOM to %f", pos.getY());
      view_.setY2(pos.getY());
      requireRepaint();
    }
    if (left) {
      LOG.i("Set LEFT to %f", pos.getX());
      view_.setX1(pos.getX());
      requireRepaint();
    }
    if (right) {
      LOG.i("Set RIGHT to %f", pos.getX());
      view_.setX2(pos.getX());
      requireRepaint();
    }
  } else {
    switch (index) {
      case Index::PARTIAL:
        setPartialItemsVisible(view_.togglePartial());
        requireRepaint();
        break;
      case Index::OK:
        if (saveCallback_) {
          saveCallback_(*this);
        }
        goUp();
        break;
      case Index::CANCEL:
        goUp();
        break;
      default:
        break;
    }
  }
  return false;
}
