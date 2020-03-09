#include "loggerfactory.h"

 std::map<std::string, Logger*> LoggerFactory::loggers_;

//------------------------------------------------------------------------------
Logger& LoggerFactory::getLogger(const std::string module) 
//------------------------------------------------------------------------------
{
  Logger* logger = loggers_[module];
  if (!logger) {
    logger = new Logger(module);
  }
  return *logger;
}