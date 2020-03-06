#include "menuitem.h"

//------------------------------------------------------------------------------
MenuItem::MenuItem(std::string name)
    : name_(name),
      subMenuOnly_(false),
      enabled_(true),
      parent_(NULL),
      items_(),
      active_(this),
      requireRepaint_(true),
      renderCallback_(),
      buttonPushCallback_(),
      // encoderPushCallback_(),
      indexChangedCallback_(),
      incrementor_(4),
      selectionChangedCallback_(),
      selector_(0, 0, 0, true)
//------------------------------------------------------------------------------
{
  incrementor_.onIndexChange([this](int8_t upDown) {
    if (indexChangedCallback_) {
      indexChangedCallback_(*this, upDown);
    }
    if (upDown == 1) {
      selector_.up();
    } else if (upDown == -1) {
      selector_.down();
    }
  });

  selector_.onSelectionChanged([this](int16_t from, int16_t to) {
    bool repaintRequired = true;
    if (selectionChangedCallback_) {
      repaintRequired = selectionChangedCallback_(*this, from, to);
    }
    if (repaintRequired) {
      requireRepaint();
    }
  });

  selector_.onIsEnabled([this](int16_t index) {
    // index validation because exception on menuitems without subitems
    if (index < items_.size()) {
      return items_[index]->isEnabled();
    } else {
      return false;
    }
  });
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::add(MenuItem* newItem)
//------------------------------------------------------------------------------
{
  newItem->setParent(this);
  items_.push_back(newItem);
  selector_.setMax(items_.size() - 1);
  return newItem;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::addAsActive(MenuItem* newItem)
//------------------------------------------------------------------------------
{
  active_ = newItem;
  return add(newItem);
}

//------------------------------------------------------------------------------
bool MenuItem::isLeaf() const
//------------------------------------------------------------------------------
{
  return items_.empty();
}

//------------------------------------------------------------------------------
void MenuItem::goUp()
//------------------------------------------------------------------------------
{
  MenuItem* current = getParent();
  while (current) {
    if (current->isSubMenuOnly()) {
      current = current->getParent();
    } else {
      parent_->setActiveItem(-1);
      break;
    }
  }
}

//------------------------------------------------------------------------------
bool MenuItem::setActiveItem(int16_t index)
//------------------------------------------------------------------------------
{
  if (index == -1) {
    active_ = this;
    active_->requireRepaint();
    return true;
  } else {
    if (items_.size() > index) {
      active_ = items_[index];
      active_->requireRepaint();
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
bool MenuItem::setActiveItem(std::string name)
//------------------------------------------------------------------------------
{
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

//------------------------------------------------------------------------------
MenuItem* MenuItem::getActiveItem()
//------------------------------------------------------------------------------
{
  return active_;
}

//------------------------------------------------------------------------------
const MenuItem* MenuItem::getActiveItem() const
//------------------------------------------------------------------------------
{
  return active_;
}

//------------------------------------------------------------------------------
bool MenuItem::isActive() const
//------------------------------------------------------------------------------
{
  return this == active_;
}

//------------------------------------------------------------------------------
std::string MenuItem::getActivePath()
//------------------------------------------------------------------------------
{
  if (isActive()) {
    return name_;
  } else {
    return name_ + "/" + active_->getActivePath();
  }
}

//------------------------------------------------------------------------------
const std::string& MenuItem::getName() const
//------------------------------------------------------------------------------
{
  return name_;
}

//------------------------------------------------------------------------------
const std::vector<MenuItem*>& MenuItem::getItems() const

//------------------------------------------------------------------------------
{
  return items_;
}

//------------------------------------------------------------------------------
const Selector& MenuItem::getSelector() const { return selector_; }

//------------------------------------------------------------------------------
Selector& MenuItem::getSelector()
//------------------------------------------------------------------------------
{
  return selector_;
}

//------------------------------------------------------------------------------
void MenuItem::encoderChanged(int16_t diff)
//------------------------------------------------------------------------------
{
  if (isActive()) {
    incrementor_.add(diff);
    // bool changeActive = true;
    // if (encoderPushCallback_) {
    //   changeActive = encoderPushCallback_(*this, diff);
    // }
    // if (changeActive) {
    // counter_.addToPos(diff);
    // }
  } else {
    active_->encoderChanged(diff);
  }
}

//------------------------------------------------------------------------------
void MenuItem::buttonPushed()
//------------------------------------------------------------------------------
{
  if (isActive()) {
    bool changeActive = true;
    if (buttonPushCallback_) {
      changeActive = buttonPushCallback_(*this);
    }
    if (changeActive && setActiveItem(selector_.getIndex())) {
      requireRepaint();
    }
  } else {
    active_->buttonPushed();
  }
}

//------------------------------------------------------------------------------
void MenuItem::loop()
//------------------------------------------------------------------------------
{
  if (isActive()) {
    if (requireRepaint_ && renderCallback_) {
      renderCallback_(*this);
      requireRepaint_ = false;
    }
  } else {
    active_->loop();
  }
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::onRender(RenderCallback_t cb)
//------------------------------------------------------------------------------
{
  renderCallback_ = cb;
  return this;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::onButtonPushed(ButtonPushCallback_t cb)
//------------------------------------------------------------------------------
{
  buttonPushCallback_ = cb;
  return this;
}

// //------------------------------------------------------------------------------
// MenuItem* MenuItem::onEncoderPushed(EncoderPushCallback_t cb)
// //------------------------------------------------------------------------------
// {
//   encoderPushCallback_ = cb;
//   return this;
// }

//------------------------------------------------------------------------------
MenuItem* MenuItem::onIndexChanged(IndexChangedCallback_t cb)
//------------------------------------------------------------------------------
{
  indexChangedCallback_ = cb;
  return this;
}
//------------------------------------------------------------------------------
MenuItem* MenuItem::onSelectionChanged(SelectionChangedCallback_t cb)
//------------------------------------------------------------------------------
{
  selectionChangedCallback_ = cb;
  return this;
}

//------------------------------------------------------------------------------
void MenuItem::requireRepaint()
//------------------------------------------------------------------------------
{
  requireRepaint_ = true;
}

//------------------------------------------------------------------------------
void MenuItem::setParent(MenuItem* parent)
//------------------------------------------------------------------------------
{
  parent_ = parent;
}

//------------------------------------------------------------------------------
bool MenuItem::isEnabled() const
//------------------------------------------------------------------------------
{
  return enabled_;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::setEnabled(bool enabled)
//------------------------------------------------------------------------------
{
  requireRepaint();
  enabled_ = enabled;
  return this;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::enable()
//------------------------------------------------------------------------------
{
  setEnabled(true);
  return this;
}
//------------------------------------------------------------------------------
MenuItem* MenuItem::disable()
//------------------------------------------------------------------------------
{
  setEnabled(false);
  return this;
}

//------------------------------------------------------------------------------
bool MenuItem::isSubMenuOnly()
//------------------------------------------------------------------------------
{
  return subMenuOnly_;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::setSubMenuOnly(bool subMenuOnly)
//------------------------------------------------------------------------------
{
  subMenuOnly_ = subMenuOnly;
  return this;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::subMenuOnly()
//------------------------------------------------------------------------------
{
  return setSubMenuOnly(true);
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::getParent()
//------------------------------------------------------------------------------
{
  return parent_;
}

//------------------------------------------------------------------------------
const MenuItem* MenuItem::getParent() const
//------------------------------------------------------------------------------
{
  return parent_;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::operator[](int16_t index)
//------------------------------------------------------------------------------
{
  if (index == -1) {
    return this;
  }

  if (index < items_.size()) {
    return items_[index];
  }

  return NULL;
}

//------------------------------------------------------------------------------
MenuItem* MenuItem::operator[](std::string name)
//------------------------------------------------------------------------------
{
  for (uint8_t i = 0; i < items_.size(); ++i) {
    MenuItem* item = items_[i];
    if (name == item->getName()) {
      return item;
    }
  }
  return NULL;
}