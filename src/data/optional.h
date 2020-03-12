#pragma once

#include <functional>

template <typename T>
class Optional {
 public:
  Optional() : isSet_(false) {}
  Optional(T value) : isSet_(true), value_(value) {}

  void set(T value) { value_ = value; }
  T change(T value) {
    T oldValue = value_;
    value_ = value;
    return oldValue;
  }
  const T get() const { return value_; }

  void clear() { isSet_ = false; }

  operator bool() const { return isSet_; }

  T* operator->() { return &value_; }
  const T* operator->() const { return &value_; }

  Optional& operator=(T value) {
    value_ = value;
    return *this;
  }

  const T& operator*() const& { return value_; }
  T& operator*() & { return value_; }

  T&& operator*() && { return *value_; }
  const T&& operator*() const&& { return *value_; }

  bool operator==(const Optional<T>& b) const {
    if (isSet_ && b.isSet_) {
      return (value_ == b.value_);
    } else {
      return (isSet_ == b.isSet_);
    }
  }
  bool operator==(const T& b) const { return isSet_ ? value_ == b : false; }

 private:
  bool isSet_;
  T value_;
};