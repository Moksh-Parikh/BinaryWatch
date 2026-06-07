#include "headers/model.h"
#include "headers/state.h"
#include "headers/charliePlexing.h"
#include <stdint.h>

void fillBufferWithTime(uint8_t hours, uint8_t minutes) {
  uint8_t adjustedHours = hours < NUMBER_OF_HOUR_LEDS ? hours : hours - NUMBER_OF_HOUR_LEDS;
  
  charlieBuffer[0] = hourLEDS[adjustedHours];
  charlieBufferSize = 1;

  for (int i = 0; i < NUMBER_OF_MINUTE_LEDS; i++) {
    if ( ( minutes & (1 << i) ) == (1 << i) ) {
      charlieBuffer[charlieBufferSize] = minuteLEDS[i];
      charlieBufferSize++;
    }
  }
}

void rewriteBufferMinutes(uint8_t minutes) {
  charlieBufferSize = BUFFER_MINUTE_OFFSET;
  for (int i = 0; i < NUMBER_OF_MINUTE_LEDS; i++) {
    if ( ( minutes & (1 << i) ) == (1 << i) ) {
      charlieBuffer[charlieBufferSize] = minuteLEDS[i];
      charlieBufferSize++;
    }
  }
}

void rewriteBufferHours(uint8_t hours) {
  uint8_t adjustedHours = hours < NUMBER_OF_HOUR_LEDS ? hours : hours - NUMBER_OF_HOUR_LEDS;
  
  charlieBuffer[0] = hourLEDS[adjustedHours];
}

void incrementDisplayMode() {
  uint16_t currentMode = GET_VALUE(clicksAndFlags, DISPLAY_MODE);
  currentMode = currentMode < 3 ? currentMode + 1 : 0;
  clicksAndFlags &= ~DISPLAY_MODE_MASK;
  clicksAndFlags |= (currentMode << DISPLAY_MODE_OFFSET);

  switch (currentMode) {
    case FULL_TIME:
    case MINUTES_ONLY:
    case TIME_SET:
    case TIMER:
      updateTimeBuffer(currentMode);
      INDICATE;
      break;
    default:
      break;
  }
}

void modelUpdate(int8_t clicks, uint8_t clickGap) {
  if (clickGap < MAX_CLICK_GAP) return;

  if (clicks == 0) {
    CLEAR_VALUE(otherFlags, INDICATION_FLAG);
  }
  else if (clicks > 0) {
    uint8_t displayMode = GET_VALUE(clicksAndFlags, DISPLAY_MODE);

    switch (clicks) {
      case 1:
        if (displayMode == TIME_SET || displayMode == TIMER) {
          switch (GET_VALUE(otherFlags, TIME_SET_FIELD)) {
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
          switch (GET_VALUE(otherFlags, TIME_SET_FIELD)) {
              case 0:
                  otherFlags |= TIME_SET_FIELD_MASK;
                  break;
              case 1:
                  CLEAR_VALUE(otherFlags, TIME_SET_FIELD);
                  incrementDisplayMode();
                  break;
            }
        }
        else {
          incrementDisplayMode();
        }
        
        break;
      case 3:
        clicksAndFlags &= ~DISPLAY_MODE_MASK;
        break;
    }
    CLEAR_VALUE(clicksAndFlags, CLICKS);
  }
  else if (clicks == -1) { // hold
    INDICATE;
    clicksAndFlags &= ~DISPLAY_MODE_MASK; // will now start a stopwatch/stop an alarm
  }
}

void updateTimeBuffer(uint8_t displayMode) {
  switch (displayMode) {
    case TIME_SET:
    case FULL_TIME:
    case MINUTES_ONLY:
      fillBufferWithTime(
        GET_VALUE(time, HOURS),
        GET_VALUE(time, MINUTES)
      );
      break;

    case TIMER:
      fillBufferWithTime(
        GET_VALUE(timerTime, HOURS),
        GET_VALUE(timerTime, MINUTES)
      );
      break;
  }
}
