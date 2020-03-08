#pragma once

#include <functional>

#include "pano/view.h"

#include "menuitem_base.h"


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

  MenuItemBounds(Display *display, const std::string &name);
  void onSave(SaveCallback_t saveCallback);
  virtual void setPositionX(double revX) override;
  virtual void setPositionY(double revY) override;

 private:
  void setPartial(bool isPartial);
  void render(MenuItem &menu);
  void render_(bool togglePartial, bool top, bool right, bool bottom, bool left, bool ok, bool cancel);
  bool pushButtonSetBounds(MenuItem &menu);

  float posRevX_;
  float posRevY_;
  View view_;
  SaveCallback_t saveCallback_;
};