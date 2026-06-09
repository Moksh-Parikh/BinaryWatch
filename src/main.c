#include "state.h"
#include "view/view.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define DEFAULT_FLAGS (FULL_TIME << DISPLAY_MODE_OFFSET) | \
                      (1 << DISPLAY_STATE_OFFSET)

volatile uint8_t flagSet1 = 0, flagSet2 = 0, flagSet3 = 0;
volatile uint16_t time = 0;
volatile uint16_t timerTime = 0;
volatile uint16_t stopwatchTime = 0;
volatile int ticks = 0;

static inline void initTimers() {
    TCCR0A = 0;
    TCCR0B = 0;

    TCCR1 = 0;

    // 8,000,000 cycles per second / 1024 = 7812.5 per second
    // 7812.5 / 1000 = 7 or 8 per millisecond
    OCR0A = 8;
    OCR1A = 20;
    OCR1C = OCR1A;

    // set the clock presaler to 1024 and enter CTC mode
    TCCR0A |= (1 << WGM01);
    TCCR0B |= (1 << CS00) | (1 << CS02);

    // enter CTC and set prescaler to 16384
    TCCR1 |= (1 << CTC1);
    TCCR1 |= (1 << CS10) | (1 << CS11) | (1 << CS12) | (1 << CS13);

    TIMSK |= (1 << OCIE0A);
    TIMSK |= (1 << OCIE1A);
}

int main(void) {
  time |= (6 << HOURS_OFFSET);
  time |= (12 << MINUTES_OFFSET);
  time |= 45;
  flagSet2 = DEFAULT_FLAGS;
  timerTime = (6 << HOURS_OFFSET) | (13 << MINUTES_OFFSET);

  fillBufferWithTime(
    GET_VALUE(time, HOURS),
    GET_VALUE(time, MINUTES)
  );

  initTimers();

  sei();

  while(1);

  return 0;
}
