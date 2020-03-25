#pragma once

typedef enum {
  UNINITIALIZED,
  INITAILIZED,
  MOVE,
  WAIT_AFTER_MOVE,
  FOCUS,
  TRIGGER,
  WAIT_BETWEEN_SHOTS,
  WAIT_AFTER_LAST_SHOT,
  FINISH
} AutomatState_t;

const char* stateToName(AutomatState_t state);