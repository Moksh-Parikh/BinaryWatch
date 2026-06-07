#include "headers/state.h"
#include "headers/charliePlexing.h"
#include "headers/model.h"

#define TICKS_PER_SEC 23

#define DEFAULT_FLAGS (FULL_TIME << DISPLAY_MODE_OFFSET) | \
                      (1 << DISPLAY_STATE_OFFSET)

volatile uint16_t clicksAndFlags = 0;
volatile uint16_t time = 0;
volatile uint16_t timerTime = 0;
volatile uint16_t stopwatchTime = 0;
volatile uint8_t otherFlags = 0;
volatile int ticks = 0;

void removeElement(charliePlexPair value) {
  int index = -1;

  for(int i = 0; i < CHARLIE_BUFFER_MAX_SIZE; i++){
    if(charlieBuffer[i].pin1 == value.pin1 &&
        charlieBuffer[i].pin2 == value.pin2
    ) {
        index = i;
        break;
    }
  }

  if(index != -1) {
    for(int i = index; i < CHARLIE_BUFFER_MAX_SIZE; i++) {
      charlieBuffer[i].pin1 = charlieBuffer[i + 1].pin1;
      charlieBuffer[i].pin2 = charlieBuffer[i + 1].pin2;
    }
  }
}

void handleClicks() {
  uint8_t clickGap = GET_VALUE(clicksAndFlags, CLICK_GAP);
  uint8_t clicks = GET_VALUE(clicksAndFlags, CLICKS);

  switch (GET_VALUE(clicksAndFlags, BUTTON_HOLD)) {
    case 0:
      if (clickGap >= CLICK_CLEAR_GAP) {
        CLEAR_VALUE(clicksAndFlags, CLICKS);
      }
      if (clickGap >= MAX_CLICK_GAP) modelUpdate(clicks);
      break;
    case 1:
      if (clicks == 0 && clickGap == MAX_CLICK_GAP) modelUpdate(-1);
      break;
  }
}

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
    uint8_t displayMode = GET_VALUE(clicksAndFlags, DISPLAY_MODE);
    
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

ISR(TIMER0_COMPA_vect) {
  cli();

  if (GET_VALUE(clicksAndFlags, DISPLAY_STATE) == 1) {
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

static inline void initTimer() {
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

    sei();
}

int main(void) {
  time |= (6 << HOURS_OFFSET);
  time |= (12 << MINUTES_OFFSET);
  time |= 45;
  clicksAndFlags = DEFAULT_FLAGS;
  timerTime = (6 << HOURS_OFFSET) | (13 << MINUTES_OFFSET);

  fillBufferWithTime(
    GET_VALUE(time, HOURS),
    GET_VALUE(time, MINUTES)
  );

  initTimer();

  while(1);

  return 0;
}
