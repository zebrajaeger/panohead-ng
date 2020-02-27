#pragma once

#include <Arduino.h>

#include "pano/picture.h"
#include "pano/position.h"
#include "pano/view.h"

class PanoCalculator {
 public:
  Raster* createMatrixRasterForPano(const View& view, const Picture& panoPicture) {
    // pano size
    double panoWidth = view.width();
    double panoHeight = view.height();

    // pic size without overlap
    double picWidth = panoPicture.widthWithinPano();
    double picHeight = panoPicture.heightWithinPano();

    // pic count
    double countX = panoWidth / picWidth;
    double countY = panoHeight / picHeight;
    uint32_t pictureCountX = (countX == (uint32_t)countX) ? countX : countX + 1;
    uint32_t pictureCountY = (countY == (uint32_t)countY) ? countY : countY + 1;

    // real overlap
    double realPictureWidthWithinPano = panoWidth / (double)pictureCountX;
    double realOverlapWidth = panoPicture.width() - realPictureWidthWithinPano;

    double realPictureHeightWithinPano = panoHeight / (double)pictureCountY;
    double realOverlapHeight = panoPicture.height() - realPictureHeightWithinPano;

    // calculate matrix
    double startX = view.getX1() - (realOverlapWidth / 2.0);
    double startY = view.getY1() - (realOverlapHeight / 2.0);
    Position startPos(startX, startY);
    Picture realPanoPicture(realPictureWidthWithinPano + realOverlapWidth, realPictureHeightWithinPano + realOverlapHeight,
                            realOverlapWidth, realOverlapHeight);
    return new Matrix(startPos, realPanoPicture, pictureCountX, pictureCountY);
  }
};