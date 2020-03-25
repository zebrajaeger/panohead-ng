#include "automat_state.h"

const char* stateToName(AutomatState_t state) {
  switch (state) {
    case AutomatState_t::UNINITIALIZED:
      return "UNINITIALIZED";
      break;
    case AutomatState_t::INITAILIZED:
      return "INITAILIZED";
      break;
    case AutomatState_t::MOVE:
      return "MOVE";
      break;
    case AutomatState_t::WAIT_AFTER_MOVE:
      return "WAIT_AFTER_MOVE";
      break;
    case AutomatState_t::FOCUS:
      return "FOCUS";
      break;
    case AutomatState_t::TRIGGER:
      return "TRIGGER";
      break;
    case AutomatState_t::WAIT_BETWEEN_SHOTS:
      return "WAIT_BETWEEN_SHOTS";
      break;
    case AutomatState_t::WAIT_AFTER_LAST_SHOT:
      return "WAIT_AFTER_LAST_SHOT";
      break;
    case AutomatState_t::FINISH:
      return "FINISH";
      break;
  }
  return "?unknown?";
}