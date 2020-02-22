#pragma once

#include <Arduino.h>

#include "pano/pano.h"

class PanoCalculator {
 public:
  pano::Raster* createMatrixRasterForPano(const pano::View& view, const pano::Picture& panoPicture) {
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
    double startX = view.x1() - (realOverlapWidth / 2.0);
    double startY = view.y1() - (realOverlapHeight / 2.0);
    pano::Position startPos(startX, startY);
    pano::Picture realPanoPicture(realPictureWidthWithinPano + realOverlapWidth, realPictureHeightWithinPano + realOverlapHeight,
                                      realOverlapWidth, realOverlapHeight);
    return new pano::Matrix(startPos, realPanoPicture, pictureCountX, pictureCountY);
  }

 private:
};