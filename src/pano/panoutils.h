#pragma once

#include <Arduino.h>

class PanoUtils {
 public:
  static double revToDeg(double rev) { return rev * (double)360; }
  static double degToRev(double deg) { return deg / (double)360; }
};
