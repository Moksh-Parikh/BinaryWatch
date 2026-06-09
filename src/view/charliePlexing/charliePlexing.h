#pragma once

#include <stdint.h>

#define NUMBER_OF_HOUR_LEDS 12
#define NUMBER_OF_MINUTE_LEDS 6
#define CHARLIE_BUFFER_MAX_SIZE NUMBER_OF_HOUR_LEDS + NUMBER_OF_MINUTE_LEDS + 1

#define APPEND_CHARLIEBUFFER(val) charlieBuffer[charlieBufferSize++] = (val);

#define BUFFER_MINUTE_OFFSET 1

#define GET_PIN1(pair) ((pair) & 0x07)
#define GET_PIN2(pair) ((pair) >> 3)

extern volatile uint8_t charlieBufferSize;
extern volatile uint8_t charlieBufferPos;
extern volatile uint8_t charlieBuffer[CHARLIE_BUFFER_MAX_SIZE];

extern uint8_t hourLEDS[NUMBER_OF_HOUR_LEDS];

extern uint8_t minuteLEDS[NUMBER_OF_MINUTE_LEDS];

extern uint8_t indicatorLED;
extern uint8_t alarmLED;

void buttonHandler();
void charlieRender(uint8_t sliceStart, uint8_t sliceEnd);
