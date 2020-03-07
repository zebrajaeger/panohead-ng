#pragma once

#include <Arduino.h>
#include <functional>

class Incrementor {
 public:
  enum Step { UP = 1, DOWN = -1 };

  /**
   * @param indexDiff +1 or -1
   */
  typedef std::function<void(Step step)> IndexChangeCallback_t;

  Incrementor(uint16_t sensitivity);
  void reset();
  int16_t getIndex();
  void onIndexChange(IndexChangeCallback_t cb);
  bool incPos();
  bool decPos();
  void add(int16_t diff);

 private:
  int16_t sensitivity_;
  int16_t pos_;
  IndexChangeCallback_t indexChangeCallback_;
};