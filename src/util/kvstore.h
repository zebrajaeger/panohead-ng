#pragma once

#include <Arduino.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <functional>

#include "util/loggerfactory.h"

class KVStore {
 public:
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

  typedef std::function<void(const kv32_t& value)> GetCallback32_t;
  typedef std::function<void(const kv64_t& value)> GetCallback64_t;

  KVStore() : LOG(LoggerFactory::getLogger("KVStore")), handle_(0) {}

  ~KVStore() { end(); }

  bool begin(const char* name) {
    if (handle_) {
      LOG.e("Already opened");
      return 0;
    }

    esp_err_t err;

    // initialize;
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      // NVS partition was truncated and needs to be erased
      // Retry nvs_flash_init
      LOG.i("Storage needs to be erased");
      err = nvs_flash_erase();
      if (err == ESP_OK) {
        LOG.i("Storage erased");
      } else {
        LOG.e("Storage not erased. Reason: %s", esp_err_to_name(err));
      }
      err = nvs_flash_init();
    }

    if (err == ESP_OK) {
      LOG.i("Storage initialized");

      // open
      err = nvs_open(name, NVS_READWRITE, &handle_);
      if (err == ESP_OK) {
        LOG.i("Storage opened");
        return true;
      } else {
        LOG.e("Storage not opened. Reason: %s", esp_err_to_name(err));
      }
    } else {
      LOG.e("Storage not initializued. Reason: %s", esp_err_to_name(err));
    }
    return false;
  }

  void end() {
    if (handle_) {
      nvs_close(handle_);
    }
    handle_ = 0;
  }

  bool get32(const char* key, kv32_t& value) {
    if (handle_) {
      esp_err_t err = nvs_get_u32(handle_, key, &(value.u));
      if (err == ESP_OK) {
        LOG.d("(G32-1)Got value for key: '%s'", key);
        return true;
      } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        LOG.w("(G32-2)Value for key: '%s' not found.");
      } else {
        LOG.e("(G32-3)Failed to read value for key: '%s'. Reason: %s", key, esp_err_to_name(err));
      }
    }
    return false;
  }
  void get32(const char* key, GetCallback32_t cb) {
    kv32_t value;
    if (get32(key, value)) {
      cb(value);
    }
  }

  bool get64(const char* key, kv64_t& value) {
    if (handle_) {
      esp_err_t err = nvs_get_u64(handle_, key, &(value.u));
      if (err == ESP_OK) {
        LOG.d("(G64-1)Got value for key: '%s'", key);
        return true;
      } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        LOG.w("(G64-2)Value for key: '%s' not found.");
      } else {
        LOG.e("(G64-3)Failed to read value for key: '%s'. Reason: %s", key, esp_err_to_name(err));
      }
    }
    return false;
  }

  void get64(const char* key, GetCallback64_t cb) {
    kv64_t value;
    if (get64(key, value)) {
      cb(value);
    }
  }

  bool set32(const char* key, kv32_t value) {
    if (handle_) {
      kv32_t temp;
      esp_err_t err = nvs_get_u32(handle_, key, &(temp.u));
      if (err == ESP_OK) {
        LOG.d("(S32-1)Found value for key: '%s'", key);
        err = nvs_set_u32(handle_, key, value.u);
        if (err == ESP_OK) {
          LOG.d("(S32-1)Wrote value for key: '%s'", key);
        } else {
          LOG.e("(S32-1)Value not written. Reason: %s", esp_err_to_name(err));
        }
      } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        LOG.d("(S32-2)Could not find value for key: '%s'", key);
        err = nvs_set_u32(handle_, key, value.u);
        if (err == ESP_OK) {
          LOG.d("(S32-2)Wrote value for key: '%s'", key);
        } else {
          LOG.e("(S32-2)Value not written. Reason: %s", esp_err_to_name(err));
        }
      } else {
        LOG.e("(S32-3)Value not written. Failed to read value for key: '%s'. Reason: %s", key, esp_err_to_name(err));
      }
    }
    return false;
  }

  bool set64(const char* key, kv64_t value) {
    if (handle_) {
      kv64_t temp;
      esp_err_t err = nvs_get_u64(handle_, key, &(temp.u));
      if (err == ESP_OK) {
        LOG.d("(S64-1)Found value for key: '%s'", key);
        err = nvs_set_u64(handle_, key, value.u);
        if (err == ESP_OK) {
          LOG.d("(S64-1)Wrote value for key: '%s'", key);
        } else {
          LOG.e("(S64-1)Value not written. Reason: %s", esp_err_to_name(err));
        }
      } else if (err == ESP_ERR_NVS_NOT_FOUND) {
        LOG.d("(S64-2)Could not find value for key: '%s'", key);
        err = nvs_set_u64(handle_, key, value.u);
        if (err == ESP_OK) {
          LOG.d("(S64-2)Wrote value for key: '%s'", key);
        } else {
          LOG.e("(S64-2)Value not written. Reason: %s", esp_err_to_name(err));
        }
      } else {
        LOG.e("(S64-3)Value not written. Failed to read value for key: '%s'. Reason: %s", key, esp_err_to_name(err));
      }
    }
    return false;
  }

  bool commit() {
    esp_err_t err;
    err = nvs_commit(handle_);
    if (err == ESP_OK) {
      LOG.d("Commit OK.");
      return true;
    } else {
      LOG.e("Not commited. Reason: %s", esp_err_to_name(err));
    }
    return false;
  }

 private:
  Logger& LOG;
  nvs_handle handle_;
};