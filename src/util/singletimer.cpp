#include "singletimer.h"

//------------------------------------------------------------------------------
SingleTimer::SingleTimer(const String& name)
    : LOG("SingleTimer(" + name + ")"),
      running_(false)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
void SingleTimer::startS(double periodS, bool strict, bool recurrent)
//------------------------------------------------------------------------------
{
  startUs(periodS * 1000000.0, strict, recurrent);
}

//------------------------------------------------------------------------------
void SingleTimer::startS(double periodS, bool strict, bool recurrent, callback_t cb)
//------------------------------------------------------------------------------
{
  startUs(periodS * 1000000.0, strict, recurrent, cb);
}

//------------------------------------------------------------------------------
void SingleTimer::startMs(uint64_t periodMs, bool strict, bool recurrent)
//------------------------------------------------------------------------------
{
  startUs(periodMs * 1000, strict, recurrent);
}

//------------------------------------------------------------------------------
void SingleTimer::startMs(uint64_t periodMs, bool strict, bool recurrent, callback_t cb)
//------------------------------------------------------------------------------
{
  startUs(periodMs * 1000, strict, recurrent, cb);
}

//------------------------------------------------------------------------------
void SingleTimer::startUs(uint64_t periodUs, bool strict , bool recurrent)
//------------------------------------------------------------------------------
{
  periodUs_ = periodUs;
  strict_ = strict;
  recurrent_ = recurrent;
  nextEvent_ = micros() + periodUs;
  running_ = true;
}

//------------------------------------------------------------------------------
void SingleTimer::startUs(uint64_t periodUs, bool strict, bool recurrent, callback_t cb)
//------------------------------------------------------------------------------
{
  periodUs_ = periodUs;
  strict_ = strict;
  recurrent_ = recurrent;
  nextEvent_ = micros() + periodUs;
  cb_ = cb;
  running_ = true;
}

//------------------------------------------------------------------------------
void SingleTimer::stop()
//------------------------------------------------------------------------------
{
  running_ = false;
}

//------------------------------------------------------------------------------
void SingleTimer::onTimer(callback_t cb)
//------------------------------------------------------------------------------
{
  cb_ = cb;
}

//------------------------------------------------------------------------------
bool SingleTimer::isRunning()
//------------------------------------------------------------------------------
{
  return running_;
}

//------------------------------------------------------------------------------
uint32_t SingleTimer::getMsToNextEvent()
//------------------------------------------------------------------------------
{
  return (nextEvent_ - micros()) / 1000;
}

//------------------------------------------------------------------------------
void SingleTimer::loop()
//------------------------------------------------------------------------------
{
  if (!running_) {
    return;
  }

  uint64_t now = micros();
  if (now >= nextEvent_) {
    if (recurrent_) {
      if (strict_) {
        nextEvent_ = nextEvent_ + periodUs_;
        trigger();
        if (nextEvent_ < micros()) {
          LOG.w("Event handler needs more time than timer period length is");
        }
      } else {
        trigger();
        nextEvent_ = micros() + periodUs_;
      }
    } else {
      running_ = false;
      trigger();
    }
  }
}

//------------------------------------------------------------------------------
void SingleTimer::trigger()
//------------------------------------------------------------------------------
{
  if (cb_) {
    cb_();
  }
}