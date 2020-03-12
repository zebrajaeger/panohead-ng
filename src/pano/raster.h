
#pragma once

#include <Arduino.h>
#include <typeinfo>

#include "data/position.h"

#include "util/loggerfactory.h"

#include "picture.h"

class Raster {
 public:
  virtual ~Raster(){};
  virtual const char* getTypeName() const = 0;
  virtual uint32_t getRows() const = 0;
  virtual uint32_t getColumnsForRowIndex(uint32_t rowIndex) const = 0;
  virtual Position getPositionFor(uint32_t columnIndex, uint32_t rowIndex) const = 0;
  virtual void statistics() = 0;
  virtual bool operator==(const Raster& b) const = 0;
};

class Matrix : public Raster {
 public:
  Matrix(const Position& startPosition, const Picture& panoPicture, uint32_t pictureColumns, uint32_t pictureRows)
      : LOG(LoggerFactory::getLogger("Matrix")),
        startPosition_(startPosition),
        panoPicture_(panoPicture),
        pictureColumns_(pictureColumns),
        pictureRows_(pictureRows) {}

  virtual ~Matrix() override {}

  virtual const char* getTypeName() const { return "Matrix"; }
  virtual uint32_t getRows() const override { return pictureRows_; };
  virtual uint32_t getColumnsForRowIndex(uint32_t rowIndex) const override { return pictureColumns_; }

  virtual Position getPositionFor(uint32_t columnIndex, uint32_t rowIndex) const override {
    double x = startPosition_.getX() + (panoPicture_.getWidthWithinPano() * (double)columnIndex);
    double y = startPosition_.getY() + (panoPicture_.getHeightWithinPano() * (double)rowIndex);
    return Position(x, y);
  };

  virtual void statistics() override {
    LOG.d("# startPos x:%f, y:%f", startPosition_.getX(), startPosition_.getY());
    LOG.d("# picsCount x:%d, y:%d", pictureColumns_, pictureRows_);
  }

  virtual bool operator==(const Raster& b) const {
    if (getTypeName() == b.getTypeName()) {
      const Matrix& other = (const Matrix&)b;
      return (startPosition_ == other.startPosition_) && (panoPicture_ == other.panoPicture_) &&
             (pictureColumns_ == other.pictureColumns_) && (pictureRows_ == other.pictureRows_);
    }
    return false;
  };

 private:
  Logger& LOG;
  Position startPosition_;
  Picture panoPicture_;
  uint32_t pictureColumns_;
  uint32_t pictureRows_;
};
