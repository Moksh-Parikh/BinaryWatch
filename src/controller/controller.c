#include "controller.h"
#include "../state.h"
#include "../model/model.h"

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

ISR(PCINT0_vect) {
    cli();
    DISABLE_BUTTON_INTERRUPTS;
    DDRB = 0;
    PORTB = 0;
    TIMSK |= (1 << OCIE0A);
    PRR &= ~(1 << PRTIM0);
    flagSet2 |= MAGIC_CLICKS;
    sei();
}

void buttonHandler() {
  DDRB = 0;
  PORTB = 0;
  PORTB |= (1 << BUTTONPIN);
  if (bit_is_clear(PINB, BUTTONPIN)
  ) {
    DDRB = 0;
    PORTB = 0;
    uint8_t wasHeld = GET_VALUE(flagSet1, BUTTON_HOLD);
    
    if (GET_VALUE(flagSet1, CLICK_GAP) < 5) {
      flagSet1 |= (1 << BUTTON_HOLD_OFFSET);
    }
    if (!wasHeld)
      CLEAR_VALUE(flagSet1, CLICK_GAP);
  }
  else {
    uint8_t clickGap = GET_VALUE(flagSet1, CLICK_GAP);
    if (GET_VALUE(flagSet1, BUTTON_HOLD) &&
        clickGap >= 1 && clickGap <= 5
    ) {
      INCREMENT_CLICKS;
    }

    CLEAR_VALUE(flagSet1, BUTTON_HOLD);
  }
}

void handleClicks() {
  uint8_t clickGap = GET_VALUE(flagSet1, CLICK_GAP);
  uint8_t clicks = GET_VALUE(flagSet2, CLICKS);

  switch (GET_VALUE(flagSet1, BUTTON_HOLD)) {
    case 0:
      if (clickGap >= CLICK_CLEAR_GAP) {
        CLEAR_VALUE(flagSet2, CLICKS);
      }
      if (clickGap >= MAX_CLICK_GAP) modelUpdate(clicks);
      break;
    case 1:
      if (clicks == 0 && clickGap == MAX_CLICK_GAP) modelUpdate(-1);
      break;
  }
}
