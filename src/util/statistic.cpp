/*
 * This file is part of the ESP32-LED-Dimmer distribution (https://github.com/zebrajaeger/esp32-led-dimmer).
 * Copyright (c) 2019 Lars Brandt.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "statistic.h"

//------------------------------------------------------------------------------
Statistic::Statistic()
    : LOG(LoggerFactory::getLogger("Statistics")),
      lastMeasurementTime_(0),
      loopCount_(0),
      period_(10000000),
      cb_()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool Statistic::begin(uint64_t periodMs)
//------------------------------------------------------------------------------
{
  period_ = periodMs * 1000;
  lastMeasurementTime_ = esp_timer_get_time() + period_;
  return true;
}

//------------------------------------------------------------------------------
void Statistic::loop()
//------------------------------------------------------------------------------
{
  ++loopCount_;
  if (lastMeasurementTime_ < esp_timer_get_time()) {
    LOG.s("========== Statistic BEGIN ==========");
    printStatistic();
    loopCount_ = 0;
    lastMeasurementTime_ = lastMeasurementTime_ + period_;
    if (cb_) {
      cb_();
    }
    LOG.s("========== Statistic END ==========");
  }
}

//------------------------------------------------------------------------------
void Statistic::onStatistic(statisticCallback_t cb)
//------------------------------------------------------------------------------
{
  cb_ = cb;
}

//------------------------------------------------------------------------------
void Statistic::printStatistic()
//------------------------------------------------------------------------------
{
  uint64_t currentTime = esp_timer_get_time();
  uint64_t lastPeriodTime = lastMeasurementTime_ - period_;
  uint64_t delta = currentTime - lastPeriodTime;
  uint64_t loopsPerSecond = (loopCount_ * 1000000) / delta;
  LOG.s("# %" PRIu64 " loops in %" PRIu64 "ms (%" PRIu64 " loops/s)", loopCount_, delta, loopsPerSecond);
}
