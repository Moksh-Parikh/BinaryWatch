#include "../../state.h"
#include "charliePlexing.h"

#include <stdbool.h>
#include <avr/io.h>
#include <stdint.h>

volatile uint8_t charlieBufferSize = 0;
volatile uint8_t charlieBufferPos = 0;
volatile uint8_t charlieBuffer[CHARLIE_BUFFER_MAX_SIZE] = {0};

uint8_t hourLEDS[NUMBER_OF_HOUR_LEDS] =
{
    1,
    8,
    2,
    16,
    3,
    24,
    32,
    4,
    17,
    10,
    11,
    25
};

uint8_t minuteLEDS[NUMBER_OF_MINUTE_LEDS] =
{
    33,
    12,
    26,
    19,
    34,
    20
};

uint8_t indicatorLED = 35;
uint8_t alarmLED = 28;

void charlieRender(uint8_t sliceStart, uint8_t sliceEnd) {
  if (sliceEnd > charlieBufferSize)
    sliceEnd = charlieBufferSize;

  if (charlieBufferPos >= sliceEnd) {
    DDRB = 0;
    PORTB = 0;
    if (ALARM_FIRING) {
      DDRB |= (1 << GET_PIN1(alarmLED)) | (1 << GET_PIN2(alarmLED));
      PORTB |= (1 << GET_PIN1(alarmLED));
    }
    if (INDICATING) {
      DDRB = (1 << GET_PIN1(indicatorLED)) | (1 << GET_PIN2(indicatorLED));
      PORTB = (1 << GET_PIN1(indicatorLED));
    }
    charlieBufferPos = sliceStart;
    buttonHandler();
  }

  DDRB = 0;
  PORTB = 0;

  DDRB |= (1 << GET_PIN1(charlieBuffer[charlieBufferPos]));
  DDRB |= (1 << GET_PIN2(charlieBuffer[charlieBufferPos]));

  PORTB |= (1 << GET_PIN1(charlieBuffer[charlieBufferPos]));

  charlieBufferPos++;
}

