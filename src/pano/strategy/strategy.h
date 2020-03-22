#pragma once

#include <Arduino.h>

#include "data/picture.h"
#include "data/position.h"
#include "data/view.h"

class Strategy {
 public:
  virtual uint32_t getPositionCount(const View& view, const Picture& panoPicture) const = 0;
  virtual Position getPosition(const View& view, const Picture& panoPicture, uint32_t imageindex) const = 0;
  virtual const char* getName() const = 0;
};
