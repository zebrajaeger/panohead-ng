#include "incrementor.h"

Incrementor::Incrementor(uint16_t sensitivity) : sensitivity_(sensitivity), pos_(0) {}

void Incrementor::reset() { pos_ = 0; }

int16_t Incrementor::getIndex() { return pos_ / sensitivity_; }

void Incrementor::onIndexChange(IndexChangeCallback_t cb) { indexChangeCallback_ = cb; }

bool Incrementor::incPos() {
  int16_t oldIndex = getIndex();
  pos_++;
  int16_t newIndex = getIndex();
  bool changed = oldIndex != newIndex;
  if (changed) {
    pos_ -= sensitivity_;
    if (indexChangeCallback_) {
      indexChangeCallback_(UP);
    }
  }
  return changed;
}

bool Incrementor::decPos() {
  int16_t oldIndex = getIndex();
  pos_--;
  int16_t newIndex = getIndex();
  bool changed = oldIndex != newIndex;
  if (changed) {
    pos_ += sensitivity_;
    if (indexChangeCallback_) {
      indexChangeCallback_(DOWN);
    }
  }
  return changed;
}

void Incrementor::add(int16_t diff) {
  if (diff > 0) {
    for (int16_t i = 0; i < diff; ++i) {
      incPos();
    }
  } else if (diff < 0) {
    for (int16_t i = 0; i > diff; --i) {
      decPos();
    }
  }
}
