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

#pragma once

#include "Arduino.h"
#include <functional>

#include "util/loggerfactory.h"

class OTA {
 public:
  typedef std::function<void()> StartEndCallback_t;
  typedef std::function<void(double uploaded)> ProgressCallback_t;

  OTA();
  bool begin();
  void loop();

  bool isUpdating(){return isUpdating_;}

  void onStart(StartEndCallback_t cb) { startCallback_ = cb; }
  void onEnd(StartEndCallback_t cb) { endCallback_ = cb; }
  void onProgress(ProgressCallback_t cb) { progressCallback_ = cb; }

 private:
  Logger &LOG;
  bool isUpdating_;
  StartEndCallback_t startCallback_;
  StartEndCallback_t endCallback_;
  ProgressCallback_t progressCallback_;
};