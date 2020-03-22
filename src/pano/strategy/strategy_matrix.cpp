#include "strategy_matrix.h"

// --------------------------------------------------------------------------------
StrategyMatrix& StrategyMatrix::getInstance()
// --------------------------------------------------------------------------------
{
  static StrategyMatrix instance;
  return instance;
}

// --------------------------------------------------------------------------------
uint32_t StrategyMatrix::getPositionCount(const View& view, const Picture& panoPicture) const
// --------------------------------------------------------------------------------
{
  PrecalculationResult_t preCalculated;
  precalculate(view, panoPicture, preCalculated);
  return preCalculated.pictureCountX * preCalculated.pictureCountY;
};

// --------------------------------------------------------------------------------
Position StrategyMatrix::getPosition(const View& view, const Picture& panoPicture, uint32_t imageindex) const
// --------------------------------------------------------------------------------
{
  PrecalculationResult_t preCalculated;
  precalculate(view, panoPicture, preCalculated);

  uint32_t rowIndex = imageindex / preCalculated.pictureCountX;
  uint32_t columnIndex = imageindex % preCalculated.pictureCountX;

  double x = preCalculated.startX + (preCalculated.realPictureWidthWithinPano * (double)columnIndex);
  double y = preCalculated.startY + (preCalculated.realPictureHeightWithinPano * (double)rowIndex);

  return Position(x, y);
};

// --------------------------------------------------------------------------------
void StrategyMatrix::precalculate(const View& view, const Picture& panoPicture, PrecalculationResult_t& result) const
// --------------------------------------------------------------------------------
{
  // pano size
  double panoWidth = view.calculateWidth();
  double panoHeight = view.calculateHeight();

  // pic size without overlap
  double picWidth = panoPicture.getWidthWithinPano();
  double picHeight = panoPicture.getHeightWithinPano();

  // pic count
  double countX = panoWidth / picWidth;
  double countY = panoHeight / picHeight;
  result.pictureCountX = (countX == (uint32_t)countX) ? countX : countX + 1;
  result.pictureCountY = (countY == (uint32_t)countY) ? countY : countY + 1;

  // real overlap
  result.realPictureWidthWithinPano = panoWidth / (double)result.pictureCountX;
  result.realOverlapWidth = panoPicture.getWidth() - result.realPictureWidthWithinPano;

  result.realPictureHeightWithinPano = panoHeight / (double)result.pictureCountY;
  result.realOverlapHeight = panoPicture.getHeight() - result.realPictureHeightWithinPano;

  // calculate matrix
  result.startX = view.getX1() - (result.realOverlapWidth / 2.0);
  result.startY = view.getY1() - (result.realOverlapHeight / 2.0);
}