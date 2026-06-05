#pragma once

#include <stdint.h>

#define MAX_CLICK_GAP 5
#define CLICK_CLEAR_GAP 14

// TIME PACKING STRUCTURE
// 0000 000000 000000
// top four are hours
// middle six minutes
// bottom six seconds

#define HOURS_MASK 0xF000 // (15 << 12) upper 4 bits
#define MINUTES_MASK 0xFC0 // (63 << 6) 4 bits from top
#define SECONDS_MASK 0x3F // (63) lower 6 bits

#define HOURS_OFFSET 12
#define MINUTES_OFFSET 6
#define SECONDS_OFFSET 0

// FLAG PACKING STRUCTURE
// 0 0 000 0000000 0000
// top 2 for DISPLAY_STATE and BUTTON_HOLD
// next 3 for DISPLAY_MODE
// middle 7 for click gap
// bottom 4 for click number

#define DISPLAY_STATE_MASK 0x8000 // (1 << 15) upper bit
#define DISPLAY_STATE_OFFSET 15

#define BUTTON_HOLD_MASK 0x4000 // (1 << 14) second from top
#define BUTTON_HOLD_OFFSET 14

#define DISPLAY_MODE_MASK 0x3800 // (7 << 11)
#define DISPLAY_MODE_OFFSET 11

#define CLICKS_MASK 0xF // (15) lower 4 bits
#define CLICKS_OFFSET 0

#define CLICK_GAP_MASK 0x7F0 // (127 << 4) 
#define CLICK_GAP_OFFSET 4

#define GET_VALUE(x, y) ((x) & y##_MASK) >> (y##_OFFSET)
#define CLEAR_VALUE(x, y) (x) &= ~(y##_MASK)

#define INCREMENT_TIME uint16_t _h = GET_VALUE(time, HOURS); \
        uint16_t _m = GET_VALUE(time, MINUTES);              \
        uint16_t _s = GET_VALUE(time, SECONDS);              \
                                                             \
        if (++_s >= 60) {                                    \
            _s = 0;                                          \
            if (++_m >= 60) {                                \
                _m = 0;                                      \
                if (++_h >= 12)                              \
                    _h = 0;                                  \
            }                                                \
        }                                                    \
                                                             \
        time = (_h << HOURS_OFFSET)    |                     \
               (_m << MINUTES_OFFSET)  |                     \
               (_s << SECONDS_OFFSET);

#define INCREMENT_MINUTES uint16_t _m = GET_VALUE(time, MINUTES);         \
                                  if (++_m >= 60) {                       \
                                      _m = 0;                             \
                                  }                                       \
                                  time &= ~MINUTES_MASK;                  \
                                  time |= (_m << MINUTES_OFFSET);

#define INCREMENT_HOURS uint16_t _h = GET_VALUE(time, HOURS);             \
                                  if (++_h >= 12) {                       \
                                      _h = 0;                             \
                                  }                                       \
                                  time &= ~HOURS_MASK;                    \
                                  time |= (_h << HOURS_OFFSET);

#define INCREMENT_CLICKS uint16_t c = GET_VALUE(clicksAndFlags, CLICKS); \
                        clicksAndFlags &= ~CLICKS_MASK;                  \
                        clicksAndFlags |= c <= (0xF - 1) ? c + 1 : c;

#define INCREMENT_CLICK_GAP uint16_t f = GET_VALUE(clicksAndFlags, CLICK_GAP);              \
                         f = f <= ((CLICK_GAP_MASK >> CLICK_GAP_OFFSET) - 1) ? f + 1 : 0;   \
                         clicksAndFlags &= ~CLICK_GAP_MASK;                                 \
                         clicksAndFlags |= (f << CLICK_GAP_OFFSET);

#define TOGGLE_DISPLAY_STATE \
    (clicksAndFlags ^= DISPLAY_STATE_MASK)

#define SET_DISPLAY_MODE(x) clicksAndFlags &= ~DISPLAY_MODE_MASK;           \
                            clicksAndFlags |= ((x) << DISPLAY_MODE_OFFSET);

enum displayMode {
  MINUTES_ONLY = 0,
  FULL_TIME,
  TIME_SET,
  TIMER,
  STOPWATCH,
  DATE,
  EXTRA,
  EXTRA2
};

extern volatile uint16_t clicksAndFlags;
extern volatile uint16_t time;
extern volatile int ticks;
