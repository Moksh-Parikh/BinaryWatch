#include "model.h"
#include "state.h"
#include "charliePlexing.h"

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
      updateTimeBuffer();
      break;
    default:
      break;
  }
}

void modelUpdate(int8_t clicks, uint8_t clickGap) {
  if (clickGap < MAX_CLICK_GAP) return;
  
  if (clicks > 0) {
    switch (clicks) {
      case 1:
        if (GET_VALUE(clicksAndFlags, DISPLAY_MODE) == TIME_SET) {
          INCREMENT_MINUTES;
          updateTimeBuffer();
        }
        else {
          TOGGLE_DISPLAY_STATE;
        }
        break;
      case 2:
        if (GET_VALUE(clicksAndFlags, DISPLAY_MODE) == TIME_SET) {
          INCREMENT_HOURS;
          updateTimeBuffer();
        }
        else {
          incrementDisplayMode();
        }
        break;
      case 3:
        if (GET_VALUE(clicksAndFlags, DISPLAY_MODE) == TIME_SET) {
          incrementDisplayMode();
        }
        break;
      case 5:
        clicksAndFlags &= ~DISPLAY_MODE_MASK;
        break;
    }
    CLEAR_VALUE(clicksAndFlags, CLICKS);
  }
  else if (clicks == -1) { // hold
    indicate();
  }
}

void updateTimeBuffer() {
  fillBufferWithTime(
    GET_VALUE(time, HOURS),
    GET_VALUE(time, MINUTES)
  );
}
