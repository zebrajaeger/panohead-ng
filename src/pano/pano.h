#pragma once

#include <Arduino.h>

namespace pano {
class Position {
 public:
  //------------------------------------------------------------------------------
  Position(double x, double y)
      : x_(x),
        y_(y)
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  double getX() const
  //------------------------------------------------------------------------------
  {
    return x_;
  }

  //------------------------------------------------------------------------------
  double getY() const
  //------------------------------------------------------------------------------
  {
    return y_;
  }

 private:
  double x_;
  double y_;
};

class Rectangle {
 public:
  //------------------------------------------------------------------------------
  Rectangle(double width, double height)
      : width_(width),
        height_(height)
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  double width() const
  //------------------------------------------------------------------------------
  {
    return width_;
  }

  //------------------------------------------------------------------------------
  double height() const
  //------------------------------------------------------------------------------
  {
    return height_;
  }

 private:
  double width_;
  double height_;
};

class View {
 public:
  //------------------------------------------------------------------------------
  View(double x1, double x2, double y1, double y2)
      : x1_(x1),
        x2_(x2),
        y1_(y1),
        y2_(y2),
        isPartial_(true)
  //------------------------------------------------------------------------------
  {}
  //------------------------------------------------------------------------------
  View(double y1, double y2)
      : x1_(0),
        x2_(0),
        y1_(y1),
        y2_(y2),
        isPartial_(false)
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  double x1() const
  //------------------------------------------------------------------------------
  {
    return x1_;
  }

  //------------------------------------------------------------------------------
  double x2() const
  //------------------------------------------------------------------------------
  {
    return x2_;
  }

  //------------------------------------------------------------------------------
  double y1() const
  //------------------------------------------------------------------------------
  {
    return y1_;
  }

  //------------------------------------------------------------------------------
  double y2() const
  //------------------------------------------------------------------------------
  {
    return y2_;
  }

  //------------------------------------------------------------------------------
  double width() const
  //------------------------------------------------------------------------------
  {
    return isPartial_ ? x2_ - x1_ : 1.0;
  }

  //------------------------------------------------------------------------------
  double height() const
  //------------------------------------------------------------------------------
  {
    return y2_ - y1_;
  }

  //------------------------------------------------------------------------------
  bool isPartial() const
  //------------------------------------------------------------------------------
  {
    return isPartial_;
  }

 private:
  double x1_;
  double x2_;
  double y1_;
  double y2_;
  bool isPartial_;
};

class Picture {
 public:
  //------------------------------------------------------------------------------
  Picture(double width, double height, double overlapWidth, double overlapHeight)
      : width_(width),
        height_(height),
        overlapWidth_(overlapWidth),
        overlapHeight_(overlapHeight)
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  double width() const
  //------------------------------------------------------------------------------
  {
    return width_;
  }

  //------------------------------------------------------------------------------
  double height() const
  //------------------------------------------------------------------------------
  {
    return height_;
  }

  //------------------------------------------------------------------------------
  double widthWithinPano() const
  //------------------------------------------------------------------------------
  {
    return width_ - overlapWidth_;
  }

  //------------------------------------------------------------------------------
  double heightWithinPano() const
  //------------------------------------------------------------------------------
  {
    return height_ - overlapHeight_;
  }

  //------------------------------------------------------------------------------
  double overlapWidth() const
  //------------------------------------------------------------------------------
  {
    return overlapWidth_;
  }

  //------------------------------------------------------------------------------
  double overlapHeight() const
  //------------------------------------------------------------------------------
  {
    return overlapHeight_;
  }

 private:
  double width_;
  double height_;
  double overlapWidth_;
  double overlapHeight_;
};

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

double revToDeg(double rev) { return rev * (double)360; }
double degToRev(double deg) { return deg / (double)360; }
}
