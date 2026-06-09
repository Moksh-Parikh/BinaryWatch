#include "model.h"
#include "../state.h"
#include "../controller/controller.h"
#include "../view/view.h"

#include <avr/interrupt.h>

void checkTimer() {
    if (timerTime == time) FIRE_ALARM;
}

ISR(TIMER1_COMPA_vect) {
  cli();

  INCREMENT_CLICK_GAP;
  handleClicks();

  if (ticks >= TICKS_PER_SEC - 1) {
    ticks = 0;
    INCREMENT_TIME;
    if (STOPWATCH_RUNNING) {
        INCREMENT_STOPWATCH_TIME;
        INDICATE;
    }
    uint8_t displayMode = GET_VALUE(flagSet2, DISPLAY_MODE);
    
    if ((GET_VALUE(time, SECONDS) == 0 || GET_VALUE(stopwatchTime, SECONDS) == 0) &&
        displayMode != TIME_SET && displayMode != TIMER
    ) {
      updateTimeBuffer(displayMode);
      if (timerTime != 0) checkTimer();
    }
  }
  ticks++;

  sei();
}


void incrementDisplayMode() {
  uint8_t currentMode = GET_VALUE(flagSet2, DISPLAY_MODE);
  currentMode = currentMode < (NUMBER_OF_MODES - 1) ? currentMode + 1 : 0;
  flagSet2 &= ~DISPLAY_MODE_MASK;
  flagSet2 |= (currentMode << DISPLAY_MODE_OFFSET);

  switch (currentMode) {
    case FULL_TIME:
    case MINUTES_ONLY:
    case TIME_SET:
    case TIMER:
    case STOPWATCH:
      updateTimeBuffer(currentMode);
      // INDICATE;
      break;
    default:
      break;
  }
}

void modelUpdate(int8_t clicks) {
  if (clicks == 0) {
    CLEAR_VALUE(flagSet3, INDICATION_FLAG);
  }
  else if (clicks > 0) {
    uint8_t displayMode = GET_VALUE(flagSet2, DISPLAY_MODE);

    switch (clicks) {
      case 1:
        if (displayMode == TIME_SET || displayMode == TIMER) {
          switch (GET_VALUE(flagSet3, TIME_SET_FIELD)) {
              case 0:
                  if (displayMode == TIME_SET) INCREMENT_MINUTES;
                  else INCREMENT_MINUTES_TIMER;
                  break;
              case 1:
                  if (displayMode == TIME_SET) INCREMENT_HOURS;
                  else INCREMENT_HOURS_TIMER;
                  break;
          }
          updateTimeBuffer(displayMode);
        }
        else {
          TOGGLE_DISPLAY_STATE;
        }

        break;
      case 2:
        if (displayMode == TIME_SET || displayMode == TIMER) {
          switch (GET_VALUE(flagSet3, TIME_SET_FIELD)) {
              case 0:
                  flagSet3 |= TIME_SET_FIELD_MASK;
                  break;
              case 1:
                  CLEAR_VALUE(flagSet3, TIME_SET_FIELD);
                  incrementDisplayMode();
                  break;
            }
        }
        else {
          incrementDisplayMode();
        }
        
        break;
      case 3:
        flagSet2 &= ~DISPLAY_MODE_MASK;
        break;
    }
    CLEAR_VALUE(flagSet2, CLICKS);
  }
  else if (clicks == -1) { // hold
    if (ALARM_FIRING) CLEAR_VALUE(flagSet3, ALARM_FLAG);
    else flagSet3 ^= STOPWATCH_STATE_MASK;
  }
}
