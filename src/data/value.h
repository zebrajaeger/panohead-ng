#pragma once

#include <functional>
#include <vector>

template <typename V>
class Value {
 public:
  typedef std::function<void(const Value<V>& value)> ValueListenerCallback_t;
  typedef std::function<void(const V& value)> ValueCallback_t;

  typedef enum  { DONT_CALL, CALL_IF_VALID, CALL } InstantlyBehavior_t;

  Value() : listeners_(), value_(), isValid_(false) {}

  // get value
  V& get() { return value_; }
  void get(ValueCallback_t cb) {
    if (isValid_) {
      cb(value_);
    }
  }
  const V& get() const { return value_; }
  const V& operator*() const& { return value_; }
  V& operator*() & { return value_; }

  // set value
  void set(const V& value, bool isValid = true) {
    bool changed = (isValid_ != isValid_) || !(value_ == value);
    isValid_ = isValid;
    value_ = value;
    if (changed) {
      propagateChange();
    }
  }
  Value& operator=(const V value) {
    set(value);
    return *this;
  }
  Value& operator=(const Value<V>& value) {
    set(value.value_, value.isValid_);
    return *this;
  }

  // remove value
  void clear() {
    bool oldValue = isValid_;
    isValid_ = false;
    if (oldValue) {
      propagateChange();
    }
  }

  // notify listeners for any indirect changes
  void propagateChange() {
    for (uint8_t i = 0; i < listeners_.size(); ++i) {
      listeners_[i](*this);
    }
  }

  // check if value is set
  bool isValid() { return isValid_; }
  void setValid(bool isValid = true) { isValid_ = isValid; }
  operator bool() const { return isValid_; }

  // listeners
  void addListener(ValueListenerCallback_t listener) { listeners_.push_back(listener); }
  void addListener(InstantlyBehavior_t instantlyBehavior, ValueListenerCallback_t listener) {
    listeners_.push_back(listener);
    switch (instantlyBehavior) {
      case DONT_CALL:
        // nothing to do;
        break;
      case CALL_IF_VALID:
        if (isValid_) {
          listener(*this);
        }
        break;
      case CALL:
        listener(*this);
        break;
    }
  }
  void removeListener(ValueListenerCallback_t listener) { listeners_.remove(listener); }

  // compare
  bool operator==(const Value<V>& b) const {
    if (isValid_ && b.isValid_) {
      return (value_ == b.value_);
    } else {
      return (isValid_ == b.isValid_);
    }
  }
  bool operator==(const V& b) const { return isValid_ ? value_ == b : false; }

 private:
  std::vector<ValueListenerCallback_t> listeners_;
  V value_;
  bool isValid_;
};