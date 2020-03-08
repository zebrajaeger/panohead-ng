#pragma once

#include <functional>

#include "menuitem_base.h"


class MenuItemDelay : public MenuItemBase {
 public:
  typedef std::function<void(int32_t timeMs)> SaveCallback_t;

  enum Index { S = 0, MS = 1, OK = 2, CANCEL = 3 };

  MenuItemDelay(Display *display, const std::string &name);
  MenuItemBase *onSave(SaveCallback_t saveCallback);
  void render(MenuItem &menu);
  void renderS(MenuItem &menu);
  void renderMs(MenuItem &menu);
  bool addToMs(int16_t diff);
  void encoderS(MenuItem &self, Incrementor::Step step);
  void encoderMs(MenuItem &self, Incrementor::Step step);

 private:
  int32_t timeMs_;
  SaveCallback_t saveCallback_;
};