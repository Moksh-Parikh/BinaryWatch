#pragma once

#include <stdint.h>

#define NUMBER_OF_HOUR_LEDS 12
#define NUMBER_OF_MINUTE_LEDS 6
#define CHARLIE_BUFFER_MAX_SIZE NUMBER_OF_HOUR_LEDS + NUMBER_OF_MINUTE_LEDS + 1

#define APPEND_CHARLIEBUFFER(val) charlieBuffer[charlieBufferSize++] = (val);

#define BUFFER_MINUTE_OFFSET 1

typedef struct {
  volatile unsigned int pin1 : 3; // only has to store up to the number 4
  volatile unsigned int pin2 : 3;
} charliePlexPair;


extern volatile uint8_t charlieBufferSize;
extern volatile uint8_t charlieBufferPos;
extern volatile charliePlexPair charlieBuffer[CHARLIE_BUFFER_MAX_SIZE];

extern charliePlexPair hourLEDS[NUMBER_OF_HOUR_LEDS];

extern charliePlexPair minuteLEDS[NUMBER_OF_MINUTE_LEDS];

extern charliePlexPair indicatorLED;
extern charliePlexPair alarmLED;

void buttonHandler();
void charlieRender(uint8_t sliceStart, uint8_t sliceEnd);
