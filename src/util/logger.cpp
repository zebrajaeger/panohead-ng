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

#include "logger.h"

const char* Logger::level_str_[Loglevel::SIZE] = {"X", "F", "E", "W", "I", "S", "D", "V"};
const char* Logger::color_str_[Loglevel::SIZE] = {"\u001B[41m\u001B[30m", "\u001b[31m", "\u001b[31m", "\u001b[33m",
                                                  "\033[1;37m",           "\u001b[36m", "\u001b[35m", "\u001b[34m"};

//------------------------------------------------------------------------------
void Logger::setLoglevel(const std::string module, Loglevel loglevel)
//------------------------------------------------------------------------------
{
  LoggerFactory::getLogger(module).setLoglevel(loglevel);
}

//------------------------------------------------------------------------------
Logger::Logger(const std::string& module)
    : Print(),
      module_(module),
      loglevel_(FATAL)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
void Logger::fatal(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isFatal()) {
    printPrefix(FATAL);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(FATAL);
  }
}

//------------------------------------------------------------------------------
void Logger::error(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isError()) {
    printPrefix(ERROR);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(ERROR);
  }
}

//------------------------------------------------------------------------------
void Logger::warn(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isWarn()) {
    printPrefix(WARN);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(WARN);
  }
}

//------------------------------------------------------------------------------
void Logger::info(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isInfo()) {
    printPrefix(INFO);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(INFO);
  }
}

//------------------------------------------------------------------------------
void Logger::statistic(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isStatistic()) {
    printPrefix(STATISTIC);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(STATISTIC);
  }
}

//------------------------------------------------------------------------------
void Logger::debug(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isDebug()) {
    printPrefix(DEBUG);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(DEBUG);
  }
}

//------------------------------------------------------------------------------
void Logger::verbose(const char* msg, ...) const
//------------------------------------------------------------------------------
{
  if (isVerbose()) {
    printPrefix(VERBOSE);

    char buffer[1024];
    va_list args;
    va_start(args, msg);
    vsprintf(buffer, msg, args);
    va_end(args);
    write(&buffer[0]);

    printPostfix(VERBOSE);
  }
}

//------------------------------------------------------------------------------
size_t Logger::write(uint8_t c)
//------------------------------------------------------------------------------
{
  return Serial.write(c);
};

//------------------------------------------------------------------------------
size_t Logger::write(uint8_t c) const
//------------------------------------------------------------------------------
{
  return Serial.write(c);
};

//------------------------------------------------------------------------------
size_t Logger::write(const char* c) const
//------------------------------------------------------------------------------
{
  return Serial.write(c);
}

//------------------------------------------------------------------------------
void Logger::printPrefix(Loglevel loglevel) const
//------------------------------------------------------------------------------
{
  using namespace std::chrono;
  auto ms = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());

  auto secs = duration_cast<seconds>(ms);
  ms -= duration_cast<milliseconds>(secs);
  auto mins = duration_cast<minutes>(secs);
  secs -= duration_cast<seconds>(mins);
  auto hour = duration_cast<hours>(mins);
  mins -= duration_cast<minutes>(hour);

  Serial.print(color_str_[loglevel]);
  Serial.print("* [");
  Serial.print(level_str_[loglevel]);
  Serial.print("] [");
  Serial.printf("%d:%02d:%02d.%03d", (int16_t)hour.count(), (int16_t)mins.count(), (int16_t)secs.count(), (int16_t)ms.count());
  Serial.print("] [");
  Serial.print(module_.c_str());
  Serial.print("] - ");
}

//------------------------------------------------------------------------------
void Logger::printPostfix(Loglevel loglevel) const
//------------------------------------------------------------------------------
{
  write("\u001b[0m");
  write('\n');
}
