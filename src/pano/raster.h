
#pragma once

#include <Arduino.h>

#include "util/logger.h"

#include "position.h"
#include "picture.h"

class Raster {
 public:
  virtual ~Raster(){};
  virtual uint32_t getRows() const = 0;
  virtual uint32_t getColumnsForRowIndex(uint32_t rowIndex) const = 0;
  virtual Position getPositionFor(uint32_t columnIndex, uint32_t rowIndex) const = 0;
  virtual void statistics() = 0;
};

class Matrix : public Raster {
 public:
  //------------------------------------------------------------------------------
  Matrix(const Position& startPosition, const Picture& panoPicture, uint32_t pictureColumns, uint32_t pictureRows)
      : LOG("Matrix"),
        startPosition_(startPosition),
        panoPicture_(panoPicture),
        pictureColumns_(pictureColumns),
        pictureRows_(pictureRows)
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  virtual ~Matrix() override
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  virtual uint32_t getRows() const override
  //------------------------------------------------------------------------------
  {
    return pictureRows_;
  };

  //------------------------------------------------------------------------------
  virtual uint32_t getColumnsForRowIndex(uint32_t rowIndex) const override
  //------------------------------------------------------------------------------
  {
    return pictureColumns_;
  }

  //------------------------------------------------------------------------------
  virtual Position getPositionFor(uint32_t columnIndex, uint32_t rowIndex) const override
  //------------------------------------------------------------------------------
  {
    double x = startPosition_.getX() + (panoPicture_.widthWithinPano() * (double)columnIndex);
    double y = startPosition_.getY() + (panoPicture_.heightWithinPano() * (double)rowIndex);
    return Position(x, y);
  };

  //------------------------------------------------------------------------------
  virtual void statistics() override
  //------------------------------------------------------------------------------
  {
    LOG.d("# startPos x:%f, y:%f", startPosition_.getX(), startPosition_.getY());
    LOG.d("# picsCount x:%d, y:%d", pictureColumns_, pictureRows_);
  }

 private:
  Logger LOG;
  Position startPosition_;
  Picture panoPicture_;
  uint32_t pictureColumns_;
  uint32_t pictureRows_;
};
