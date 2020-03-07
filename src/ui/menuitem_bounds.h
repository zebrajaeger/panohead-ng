#pragma once

#include <functional>

#include "pano/view.h"
#include "pano/panoutils.h"

#include "menuitem_base.h"

#include "displayutils.h"

class MenuItemBounds : public MenuItemBase {
 public:
  typedef std::function<void(View view)> SaveCallback_t;

  enum Index {
    PARTIAL = 0,
    TOP = 1,
    TOP_RIGHT = 2,
    RIGHT = 3,
    BOTTOM_RIGHT = 4,
    BOTTOM = 5,
    BOTTOM_LEFT = 6,
    LEFT = 7,
    TOP_LEFT = 8,
    OK = 9,
    CANCEL = 10
  };

  //------------------------------------------------------------------------------
  MenuItemBounds(U8G2 *u8g2, const std::string &name)
      : MenuItemBase(u8g2, name),
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

    add(new MenuItemBase(u8g2, "TogglePartial"));
    add(new MenuItemBase(u8g2, "Top"));
    add(new MenuItemBase(u8g2, "Top Right"));
    add(new MenuItemBase(u8g2, "Right"));
    add(new MenuItemBase(u8g2, "Bottom Right"));
    add(new MenuItemBase(u8g2, "Bottom"));
    add(new MenuItemBase(u8g2, "Bottom Left"));
    add(new MenuItemBase(u8g2, "Left"));
    add(new MenuItemBase(u8g2, "Top Left"));
    add(new MenuItemBase(u8g2, "Ok"));
    add(new MenuItemBase(u8g2, "Cancel"));
  }

  //------------------------------------------------------------------------------
  void onSave(SaveCallback_t saveCallback)
  //------------------------------------------------------------------------------
  {
    saveCallback_ = saveCallback;
  }

  //------------------------------------------------------------------------------
  virtual void setPositionX(double revX) override
  //------------------------------------------------------------------------------
  {
    posRevX_ = revX;
    requireRepaint();
  }
  //------------------------------------------------------------------------------
  virtual void setPositionY(double revY) override
  //------------------------------------------------------------------------------
  {
    posRevY_ = revY;
    requireRepaint();
  }

 private:
  void setPartial(bool isPartial) {
    bool enabled = !isPartial;
    (*this)["Top Right"]->setEnabled(enabled);
    (*this)["Right"]->setEnabled(enabled);
    (*this)["Bottom Right"]->setEnabled(enabled);
    (*this)["Bottom Left"]->setEnabled(enabled);
    (*this)["Left"]->setEnabled(enabled);
    (*this)["Top Left"]->setEnabled(enabled);
  }

  //------------------------------------------------------------------------------
  void render(MenuItem &menu)
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
  void render_(bool togglePartial, bool top, bool right, bool bottom, bool left, bool ok, bool cancel)
  //------------------------------------------------------------------------------
  {
    u8g2_->clearBuffer();

    // partial / full pano
    DisplayUtils::drawSymbolAt(u8g2_, 22, 24 + 16, togglePartial, view_.isPartial() ? 0x8c : 0xf6);

    // top
    if (top) {
      u8g2_->drawBox(10, 0, 44, 10);
    } else {
      u8g2_->drawFrame(10, 0, 44, 10);
    }
    // bottom
    if (bottom) {
      u8g2_->drawBox(10, 53, 44, 10);
    } else {
      u8g2_->drawFrame(10, 53, 44, 10);
    }

    if (!view_.isPartial()) {
      // left
      if (left) {
        u8g2_->drawBox(0, 10, 10, 44);
      } else {
        u8g2_->drawFrame(0, 10, 10, 44);
      }
      // right
      if (right) {
        u8g2_->drawBox(54, 10, 10, 44);
      } else {
        u8g2_->drawFrame(54, 10, 10, 44);
      }
    }

    // pos:
    u8g2_->setFont(u8g2_font_timR10_tf);
    u8g2_->drawStr(71, 8, "x:");
    DisplayUtils::drawAngleAt(u8g2_, 87, -2, false, false, PanoUtils::revToDeg(posRevX_));
    u8g2_->drawStr(71, 22, "y:");
    DisplayUtils::drawAngleAt(u8g2_, 87, 12, false, false, PanoUtils::revToDeg(posRevY_));

    // ok
    DisplayUtils::drawSymbolAt(u8g2_, 64 + 8, 60, ok, 0x73);

    // cancel
    u8g2_->setFont(u8g2_font_open_iconic_check_2x_t);
    DisplayUtils::drawSymbolAt(u8g2_, 64 + 8 + 24, 60, cancel, 0x11b);

    u8g2_->sendBuffer();
  }

  //------------------------------------------------------------------------------
  bool pushButtonSetBounds(MenuItem &menu)
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

  float posRevX_;
  float posRevY_;
  View view_;
  SaveCallback_t saveCallback_;
};