#pragma once

#include <functional>

#include "pano/view.h"

#include "menuitem_base.h"

class MenuItemBounds : public MenuItemBase {
 public:
  typedef std::function<void(MenuItemBounds &self)> SaveCallback_t;

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

  MenuItemBounds(Display *display, const std::string &name, bool showPartialOption);

  void onSave(SaveCallback_t saveCallback) { saveCallback_ = saveCallback; }

  virtual void setPositionX(double revX) override;
  virtual void setPositionY(double revY) override;

  void setShowPartialOption(bool showPartialOption) { showPartialOption_ = showPartialOption; };
  bool isShowPartialOption() { return showPartialOption_; };
  const View &getView() const { return view_; }

 private:
  void render(MenuItem &menu);
  void render_(bool togglePartial, bool top, bool right, bool bottom, bool left, bool ok, bool cancel);
  bool pushButtonSetBounds(MenuItem &menu);
  void setPartialItemsVisible(bool isPartial);

  float posRevX_;
  float posRevY_;
  bool showPartialOption_;
  View view_;
  SaveCallback_t saveCallback_;
};