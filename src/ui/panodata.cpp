#include "panodata.h"

#include <ArduinoJson.h>

void PanoData::toJson(String& json) const {
  const size_t capacity = 3 * JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(5);
  DynamicJsonDocument doc(capacity);

  JsonObject v = doc.createNestedObject("v");
  v["l"] = view_.getX1();
  v["r"] = view_.getX2();
  v["t"] = view_.getY1();
  v["b"] = view_.getY2();
  v["p"] = view_.isPartial();

  JsonObject p = doc.createNestedObject("p");

  JsonObject p_s = p.createNestedObject("s");
  p_s["w"] = picture_.getWidth();
  p_s["h"] = picture_.getHeight();

  JsonObject p_o = p.createNestedObject("o");
  p_o["w"] = picture_.getOverlapWidth();
  p_o["h"] = picture_.getOverlapHeight();

  if (raster_) {
    JsonObject r = doc.createNestedObject("r");
    r["t"] = raster_->getTypeName();
    r["r"] = raster_->getRows();
  }

  if (shots_) {
    doc["s"] = shots_->getShotCount();
  }

  serializeJson(doc, json);
}