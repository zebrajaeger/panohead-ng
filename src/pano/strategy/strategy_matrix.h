#pragma once

#include <Arduino.h>

#include "strategy.h"

class StrategyMatrix : public Strategy {
 public:
  static StrategyMatrix& getInstance();

  virtual uint32_t getPositionCount(const View& view, const Picture& panoPicture) const;
  virtual Position getPosition(const View& view, const Picture& panoPicture, uint32_t imageindex) const;
  virtual const char* getName() const { return "Matrix"; }

 private:
  typedef struct {
    uint32_t pictureCountX;
    uint32_t pictureCountY;
    double realPictureWidthWithinPano;
    double realOverlapWidth;
    double realPictureHeightWithinPano;
    double realOverlapHeight;
    double startX;
    double startY;
  } PrecalculationResult_t;

  void precalculate(const View& view, const Picture& panoPicture, PrecalculationResult_t& result) const;
};