#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemNumber : public MenuItemBase {
 public:
  typedef std::function<void(const MenuItemNumber &self)> SaveCallback_t;

  enum Index { FULL = 0, PART = 1, OK = 2, CANCEL = 3 };

  MenuItemNumber(Display *display, const std::string &name, const std::string &postfix, int32_t maxValue);

  MenuItemNumber *onSave(SaveCallback_t saveCallback);

  void setValue(int32_t value) {
    LOG.d("'%s'.setValue(%d)", getName().c_str(), value);
    value_ = value;
    requireRepaint();
  }

  void render(MenuItem &menu);
  void renderFull(MenuItem &menu) { render_(true, false, false, false); }
  void renderPart(MenuItem &menu) { render_(false, true, false, false); }
  void render_(bool selFull, bool selPart, bool selOk, bool selCancel);

  void encoderFull(MenuItem &self, Incrementor::Step step);
  void encoderPart(MenuItem &self, Incrementor::Step step);

  bool addToValue(int16_t diff);
  int32_t getValue() const { return value_; }
  int32_t getMaxValue() const { return maxValue_; }

 private:
  int32_t value_;
  int32_t maxValue_;
  std::string postfix_;
  SaveCallback_t saveCallback_;
};