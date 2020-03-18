#pragma once

#include <functional>
#include <vector>

template <typename V>
class Value {
 public:
  // TODO transfer Value Object reference instead of Value Object's value because of validity check and notification on clear
  typedef std::function<void(const Value<V>& value)> ValueListenerCallback_t;

  Value() : listeners_(), value_(), isValid_(false) {}

  // get value
  V& get() { return value_; }
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