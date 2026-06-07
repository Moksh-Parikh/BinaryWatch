#include "headers/charliePlexing.h"
#include "headers/state.h"

#include <avr/interrupt.h>

ISR(TIMER0_COMPA_vect) {
  cli();

  if (GET_VALUE(clicksAndFlags, DISPLAY_STATE)) {
    // fillBufferWithTime(
    //   GET_VALUE(clicksAndFlags, CLICKS),
    //   GET_VALUE(clicksAndFlags, CLICK_GAP)
    // );
    switch (GET_VALUE(clicksAndFlags, DISPLAY_MODE)) {
      case MINUTES_ONLY:
        charlieRender(1, charlieBufferSize);
        break;
      case TIME_SET:
      case FULL_TIME:
      case TIMER:
      case STOPWATCH:
        charlieRender(0, charlieBufferSize);
        break;
      default:
        buttonHandler();
        break;
    }
  }
  else buttonHandler();

  sei();
}

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
    
    case STOPWATCH:
      fillBufferWithTime(
        GET_VALUE(stopwatchTime, HOURS),
        GET_VALUE(stopwatchTime, MINUTES)
      );
      break;
  }
}
