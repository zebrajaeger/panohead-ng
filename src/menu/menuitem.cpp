#include "menuitem.h"

MenuItem::MenuItem(std::string name)
    : name_(name), parent_(NULL), items_(), active_(this), requireRepaint_(true), renderCallback_(), counter_(4, 0, 0, 0, true) {
  counter_.onIndexChange([this](int16_t from, int16_t to) { requireRepaint(); });
}

void MenuItem::add(MenuItem* newItem) {
  newItem->setParent(this);
  items_.push_back(newItem);
  counter_.setMax(items_.size() - 1);
}

bool MenuItem::isLeaf() const { return items_.empty(); }

bool MenuItem::setActiveItem(int16_t index) {
  if (index == -1) {
    active_ = this;
    return true;
  } else {
    if (items_.size() > index) {
      active_ = items_[index];
      return true;
    }
  }
  return false;
}

bool MenuItem::setActiveItem(std::string name) {
  if (name == name_) {
    active_ = this;
    return true;
  } else {
    for (std::size_t i = 0; i < items_.size(); ++i) {
      MenuItem* item = items_[i];
      if (name == item->getName()) {
        active_ = item;
        return true;
      }
    }
  }
  return false;
}

MenuItem* MenuItem::getActiveItem() { return active_; }

const MenuItem* MenuItem::getActiveItem() const { return active_; }

bool MenuItem::isActive() const { return this == active_; }

std::string MenuItem::getActivePath() {
  if (isActive()) {
    return name_;
  } else {
    return name_ + "/" + active_->getActivePath();
  }
}

const std::string& MenuItem::getName() const { return name_; }
const std::vector<MenuItem*>& MenuItem::getItems() const { return items_; }

void MenuItem::encoderChanged(int16_t diff) {
  if (isActive()) {
    counter_.add(diff);
  } else {
    active_->encoderChanged(diff);
  }
}

void MenuItem::buttonPushed() {
  if (isActive()) {
    if (setActiveItem(counter_.getIndex())) {
      requireRepaint();
    }
  } else {
    active_->buttonPushed();
  }
}

void MenuItem::loop() {
  if (isActive()) {
    if (requireRepaint_ && renderCallback_) {
      renderCallback_(*this, counter_);
      requireRepaint_ = false;
    }
  } else {
    active_->loop();
  }
}

void MenuItem::onRender(RenderCallback_t cb) { renderCallback_ = cb; }

void MenuItem::requireRepaint() { requireRepaint_ = true; }

void MenuItem::setParent(MenuItem* parent) { parent_ = parent; }