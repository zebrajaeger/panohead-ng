#pragma once

#include <Arduino.h>

#include <ArduinoJson.h>

#include "data/value.h"
#include "data/view.h"
#include "pano/picture.h"
#include "pano/shot.h"

class PanoUtils {
 public:
  static double revToDeg(double rev) { return rev * (double)360; }
  static double degToRev(double deg) { return deg / (double)360; }

  static void toJson(String& json, const Value<View>& viewValue, const Value<Picture>& pictureValue, Value<Raster*> rasterValue,
                     Value<Shots*> shotsValue) {
    const size_t capacity = 3 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
    DynamicJsonDocument doc(capacity);

    const View& view = *viewValue;
    JsonObject v = doc.createNestedObject("v");
    v["l"] = view.getX1();
    v["r"] = view.getX2();
    v["t"] = view.getY1();
    v["b"] = view.getY2();
    v["p"] = view.isPartial();

    const Picture& picture = *pictureValue;
    JsonObject p = doc.createNestedObject("p");

    JsonObject p_s = p.createNestedObject("s");
    p_s["w"] = picture.getWidth();
    p_s["h"] = picture.getHeight();

    JsonObject p_o = p.createNestedObject("o");
    p_o["w"] = picture.getOverlapWidth();
    p_o["h"] = picture.getOverlapHeight();

    if (rasterValue) {
      Raster* raster = *rasterValue;
      JsonObject r = doc.createNestedObject("r");
      r["t"] = raster->getTypeName();
      r["r"] = raster->getRows();
    }

    if (shotsValue) {
      doc["s"] = (*shotsValue)->getShotCount();
    }

    serializeJson(doc, json);
  }
};
