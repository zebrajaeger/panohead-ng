#pragma once

#include <Arduino.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <functional>

#include "util/loggerfactory.h"

class KVStore {
 public:
  typedef union {
    int8_t i;
    uint8_t u;
    bool b;
  } kv8_t;

  typedef union {
    int16_t i;
    uint16_t u;
  } kv16_t;

  typedef union {
    float f;
    int32_t i;
    uint32_t u;
  } kv32_t;

  typedef union {
    double f;
    int64_t i;
    uint64_t u;
  } kv64_t;

  typedef std::function<void(const kv8_t& value)> GetCallback8_t;
  typedef std::function<void(const kv16_t& value)> GetCallback16_t;
  typedef std::function<void(const kv32_t& value)> GetCallback32_t;
  typedef std::function<void(const kv64_t& value)> GetCallback64_t;

  KVStore() : LOG(LoggerFactory::getLogger("KVStore")), handle_(0) {}
  ~KVStore() { end(); }

  bool begin(const char* name);
  void end();

  bool commit();

  bool get8(const char* key, kv8_t& value);
  void get8(const char* key, GetCallback8_t cb);
  bool set8(const char* key, kv8_t value);

  bool get16(const char* key, kv16_t& value);
  void get16(const char* key, GetCallback16_t cb);
  bool set16(const char* key, kv16_t value);

  bool get32(const char* key, kv32_t& value);
  void get32(const char* key, GetCallback32_t cb);
  bool set32(const char* key, kv32_t value);

  bool get64(const char* key, kv64_t& value);
  void get64(const char* key, GetCallback64_t cb);
  bool set64(const char* key, kv64_t value);

 private:
  Logger& LOG;
  nvs_handle handle_;
};