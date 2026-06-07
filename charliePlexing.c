#include "headers/state.h"
#include "headers/charliePlexing.h"

#include <stdbool.h>
#include <avr/io.h>

volatile uint8_t charlieBufferSize = 0;
volatile uint8_t charlieBufferPos = 0;
volatile charliePlexPair charlieBuffer[CHARLIE_BUFFER_MAX_SIZE] = {0};

charliePlexPair hourLEDS[NUMBER_OF_HOUR_LEDS] = {
                        (charliePlexPair){1, 0},
                        (charliePlexPair){0, 1},
                        (charliePlexPair){2, 0},
                        (charliePlexPair){0, 2},
                        (charliePlexPair){3, 0},
                        (charliePlexPair){0, 3},
                        (charliePlexPair){0, 4},
                        (charliePlexPair){4, 0},
                        (charliePlexPair){1, 2},
                        (charliePlexPair){2, 1},
                        (charliePlexPair){3, 1},
                        (charliePlexPair){1, 3}
                      };

charliePlexPair minuteLEDS[NUMBER_OF_MINUTE_LEDS] = {
                        (charliePlexPair){1, 4},
                        (charliePlexPair){4, 1},
                        (charliePlexPair){2, 3},
                        (charliePlexPair){3, 2},
                        (charliePlexPair){2, 4},
                        (charliePlexPair){4, 2},
                        };

charliePlexPair indicatorLED = {3, 4};
charliePlexPair alarmLED = {4, 3};

void charlieRender(uint8_t sliceStart, uint8_t sliceEnd) {
  if (sliceEnd > charlieBufferSize)
    sliceEnd = charlieBufferSize;

  if (charlieBufferPos >= sliceEnd) {
    if (ALARM_FIRING) {
      DDRB = 0;
      PORTB = 0;
      DDRB |= (1 << alarmLED.pin1) | (1 << alarmLED.pin2);
      PORTB |= (1 << alarmLED.pin1);
    }
    if (INDICATING) {
      DDRB = 0;
      PORTB = 0;
      DDRB |= (1 << indicatorLED.pin1) | (1 << indicatorLED.pin2);
      PORTB |= (1 << indicatorLED.pin1);
    }
    charlieBufferPos = sliceStart;
    buttonHandler();
  }

  DDRB = 0;
  PORTB = 0;

  DDRB |= (1 << charlieBuffer[charlieBufferPos].pin1);
  DDRB |= (1 << charlieBuffer[charlieBufferPos].pin2);

  PORTB |= (1 << charlieBuffer[charlieBufferPos].pin1);

  charlieBufferPos++;
}
