#pragma once

#include <functional>
#include <vector>

template <typename V>
class Value {
 public:
  typedef std::function<void(const V& value)> ValueListenerCallback_t;

  Value() : listeners_(), value_(), isValid_(false) {}

  V& get() { return value_; }
  const V& get() const { return value_; }
  void set(const V& value, bool isValid = true) {
    isValid_ = isValid;
    value_ = value;
    propagateChange();
  }
  const V& operator*() const& { return value_; }
  V& operator*() & { return value_; }
  void propagateChange() {
    for (uint8_t i = 0; i < listeners_.size(); ++i) {
      listeners_[i](value_);
    }
  }

  bool isValid() { return isValid_; }
  void setValid(bool isValid = true) { isValid_ = isValid; }
  operator bool() const { return isValid_; }

  void addListener(ValueListenerCallback_t listener) { listeners_.push_back(listener); }
  void removeListener(ValueListenerCallback_t listener) { listeners_.remove(listener); }

 private:
  std::vector<ValueListenerCallback_t> listeners_;
  V value_;
  bool isValid_;
};