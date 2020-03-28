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

#include <Arduino.h>
#include <Print.h>
#include <chrono>
#include <map>

#include "loggerfactory.h"

#ifndef LOGGER_BUFFER_SIZE
#define LOGGER_BUFFER_SIZE 1024
#endif

class Logger : public Print {
 public:
  enum Loglevel { OFF = 0, FATAL = 1, ERROR = 2, WARN = 3, INFO = 4, DEBUG = 5, VERBOSE = 6 };

  static void setLoglevel(const std::string module, Loglevel loglevel);

  void setLoglevel(Loglevel loglevel) { loglevel_ = loglevel; }
  Loglevel getLoglevel() { return loglevel_; }

  // fatal
  inline bool isF() const { return isFatal(); }
  inline bool isFatal() const { return loglevel_ <= FATAL; }

  template <typename... Args>
  inline void f(const char* msg, Args... args) const {
    fatal(msg, args...);
  }
  void fatal(const char* msg, ...) const;

  // error
  inline bool isE() const { return isError(); }
  inline bool isError() const { return loglevel_ <= ERROR; }

  template <typename... Args>
  inline void e(const char* msg, Args... args) const {
    error(msg, args...);
  }
  void error(const char* msg, ...) const;

  // warn
  inline bool isW() const { return isWarn(); }
  inline bool isWarn() const { return loglevel_ <= WARN; }

  template <typename... Args>
  inline void w(const char* msg, Args... args) const {
    warn(msg, args...);
  }
  void warn(const char* msg, ...) const;

  // info
  inline bool isI() const { return isInfo(); }
  inline bool isInfo() const { return loglevel_ <= INFO; }

  template <typename... Args>
  inline void i(const char* msg, Args... args) const {
    info(msg, args...);
  }
  void info(const char* msg, ...) const;

  // debug
  inline bool isD() const { return isDebug(); }
  inline bool isDebug() const { return loglevel_ <= DEBUG; }

  template <typename... Args>
  inline void d(const char* msg, Args... args) const {
    debug(msg, args...);
  }
  void debug(const char* msg, ...) const;

  // verbose
  inline bool isV() const { return isVerbose(); }
  inline bool isVerbose() const { return loglevel_ <= VERBOSE; }

  template <typename... Args>
  inline void v(const char* msg, Args... args) const {
    verbose(msg, args...);
  }
  void verbose(const char* msg, ...) const;

  virtual size_t write(uint8_t c);
  size_t write(uint8_t c) const;
  size_t write(const char* c) const;

 private:
  Logger(const std::string& module);
  friend class LoggerFactory;
  void printPrefix(Loglevel loglevel) const;
  void printPostfix(Loglevel loglevel) const;

  static const char* level_str_[7];
  static const char* color_str_[7];

  const std::string module_;
  char buffer[LOGGER_BUFFER_SIZE];
  Loglevel loglevel_;
};