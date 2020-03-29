#include "menuitem.h"

//------------------------------------------------------------------------------
MenuItem::MenuItem(const std::string& name)
    : LOG(LoggerFactory::getLogger("MenuItem")),
      name_(name),
      subMenuOnly_(false),
      enabled_(true),
      parent_(NULL),
      items_(),
      active_(this),
      requireRepaint_(true),
      renderCallback_(),
      buttonPushCallback_(),
      indexChangedCallback_(),
      incrementor_(4),
      selectionChangedCallback_(),
      selector_(0, 0, 0, true)
//------------------------------------------------------------------------------
{
  // LOG.d("Create MenuItem '%s'", name_.c_str());
  
  incrementor_.onIndexChange([this](Incrementor::Step upDown) {
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
MenuItem::~MenuItem()
//------------------------------------------------------------------------------
{
  // LOG.d("Destroy MenuItem '%s'", name_.c_str());
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
void MenuItem::goUp(bool callEnterNotification)
//------------------------------------------------------------------------------
{
  MenuItem* current = getParent();
  while (current) {
    if (current->isSubMenuOnly()) {
      current = current->getParent();
    } else {
      parent_->setActiveItem(-1, callEnterNotification);
      break;
    }
  }
}

//------------------------------------------------------------------------------
bool MenuItem::setActiveItem(int16_t index, bool callEnterNotification)
//------------------------------------------------------------------------------
{
  if (index == -1) {
    active_ = this;
    active_->requireRepaint();
    if (callEnterNotification) {
      propagateEnter();
    }
    return true;
  } else {
    if (items_.size() > index) {
      active_ = items_[index];
      active_->requireRepaint();
      if (callEnterNotification) {
        active_->propagateEnter();
      }
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
bool MenuItem::setActiveItem(std::string name, bool callEnterNotification)
//------------------------------------------------------------------------------
{
  if (name == name_) {
    active_ = this;
    active_->requireRepaint();
    if (callEnterNotification) {
      propagateEnter();
    }
    return true;
  } else {
    for (std::size_t i = 0; i < items_.size(); ++i) {
      MenuItem* item = items_[i];
      if (name == item->getName()) {
        active_ = item;
        active_->requireRepaint();
        if (callEnterNotification) {
          active_->propagateEnter();
        }
        return true;
      }
    }
  }
  return false;
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
MenuItem* MenuItem::getActivePathItem()
//------------------------------------------------------------------------------
{
  if (isActive()) {
    return this;
  } else {
    return active_->getActivePathItem();
  }
}

//------------------------------------------------------------------------------
void MenuItem::encoderChanged(int16_t diff)
//------------------------------------------------------------------------------
{
  if (isActive()) {
    incrementor_.add(diff);
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
MenuItem* MenuItem::onEnter(EnterItemCallback_t cb)
//------------------------------------------------------------------------------
{
  enterItemCallback_ = cb;
  return this;
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
MenuItem* MenuItem::setSubMenuOnly(bool subMenuOnly)
//------------------------------------------------------------------------------
{
  subMenuOnly_ = subMenuOnly;
  return this;
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

//------------------------------------------------------------------------------
void MenuItem::propagateEnter()
//------------------------------------------------------------------------------
{
  if (enterItemCallback_) {
    enterItemCallback_(*this);
  }
}