#pragma once

#include <Arduino.h>
#include <map>

#include "logger.h"

class Logger;
class LoggerFactory {
 public:

  static Logger& getLogger(const std::string module);

 private:
  static std::map<std::string, Logger*> loggers_;
};