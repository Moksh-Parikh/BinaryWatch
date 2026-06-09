#pragma once

#include <stdint.h>

#define MAX_CLICK_GAP 6
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

// FLAGSET1 PACKING STRUCTURE
// 0 0000000
// lower 7 for click gap
// upper bit for BUTTON_HOLD

#define BUTTON_HOLD_MASK 0x80 // (1 << 7) upper bit
#define BUTTON_HOLD_OFFSET 7

#define CLICK_GAP_MASK 0x7F // 127 
#define CLICK_GAP_OFFSET 0

// FLAGSET2 PACKING STRUCTURE
// 0 000 0000
// upper bit for DISPLAY_STATE
// next 3 for DISPLAY_MODE
// bottom 4 for click number

#define DISPLAY_STATE_MASK 0x80 // (1 << 7) upper bit
#define DISPLAY_STATE_OFFSET 7

#define DISPLAY_MODE_MASK 0x70 // (7 << 4)
#define DISPLAY_MODE_OFFSET 4

#define CLICKS_MASK 0xF // (15) lower 4 bits
#define CLICKS_OFFSET 0

// FLAGSET3 STRUCTURE
// 000 0 0 0 0 0
// Lower bit for indicatorLED
// second bit for alarmLED
// third bit for stopwatch state
// fourth bit for timer
// fifth bit for minutes/hours set

#define INDICATION_FLAG_MASK 0x1 // (1)
#define INDICATION_FLAG_OFFSET 0

#define ALARM_FLAG_MASK 0x2 // (1 << 1)
#define ALARM_FLAG_OFFSET 1

#define STOPWATCH_STATE_MASK 0x4 // (1 << 2)
#define STOPWATCH_STATE_OFFSET 2

#define IS_TIMER_MASK 0x8 // (1 << 3)
#define IS_TIMER_OFFSET 3

#define TIME_SET_FIELD_MASK 0x10 // (1 << 4)
#define TIME_SET_FIELD_OFFSET 4

#define GET_VALUE(x, y) ((x) & y##_MASK) >> (y##_OFFSET)
#define CLEAR_VALUE(x, y) (x) &= ~(y##_MASK)

#define IS_TIMER GET_VALUE(flagSet3, IS_TIMER)
#define IS_STOPWATCH GET_VALUE(flagSet3, IS_STOPWATCH)
#define INDICATING GET_VALUE(flagSet3, INDICATION_FLAG)
#define ALARM_FIRING GET_VALUE(flagSet3, ALARM_FLAG)
#define STOPWATCH_RUNNING GET_VALUE(flagSet3, STOPWATCH_STATE)

#define INDICATE flagSet3 |= (1 << INDICATION_FLAG_OFFSET);
#define FIRE_ALARM flagSet3 |= (1 << ALARM_FLAG_OFFSET);

#define INCREMENT_TIME do { uint16_t _h = GET_VALUE(time, HOURS); \
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
               (_s << SECONDS_OFFSET); } while (0);

#define INCREMENT_STOPWATCH_TIME do { uint16_t _h = GET_VALUE(stopwatchTime, HOURS); \
        uint16_t _m = GET_VALUE(stopwatchTime, MINUTES);              \
        uint16_t _s = GET_VALUE(stopwatchTime, SECONDS);              \
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
        stopwatchTime = (_h << HOURS_OFFSET)    |                     \
               (_m << MINUTES_OFFSET)  |                     \
               (_s << SECONDS_OFFSET); } while (0);

#define INCREMENT_MINUTES do { uint16_t _m = GET_VALUE(time, MINUTES);         \
                                  if (++_m >= 60) {                       \
                                      _m = 0;                             \
                                  }                                       \
                                  time &= ~MINUTES_MASK;                  \
                                  time |= (_m << MINUTES_OFFSET); } while(0)

#define INCREMENT_HOURS do { uint16_t _h = GET_VALUE(time, HOURS);             \
                                  if (++_h >= 12) {                       \
                                      _h = 0;                             \
                                  }                                       \
                                  time &= ~HOURS_MASK;                    \
                                  time |= (_h << HOURS_OFFSET); } while(0)

#define INCREMENT_MINUTES_TIMER do { uint16_t __m = GET_VALUE(timerTime, MINUTES);         \
                                  if (++__m >= 60) {                       \
                                      __m = 0;                             \
                                  }                                       \
                                  timerTime &= ~MINUTES_MASK;                  \
                                  timerTime |= (__m << MINUTES_OFFSET); } while(0)

#define INCREMENT_HOURS_TIMER do { uint16_t __h = GET_VALUE(timerTime, HOURS);             \
                                  if (++__h >= 12) {                       \
                                      __h = 0;                             \
                                  }                                       \
                                  timerTime &= ~HOURS_MASK;                    \
                                  timerTime |= (__h << HOURS_OFFSET); } while(0)

#define INCREMENT_CLICKS uint8_t c = GET_VALUE(flagSet2, CLICKS); \
                        flagSet2 &= ~CLICKS_MASK;                  \
                        flagSet2 |= c <= (0xF - 1) ? c + 1 : c;

#define INCREMENT_CLICK_GAP uint8_t f = GET_VALUE(flagSet1, CLICK_GAP);              \
                         f = f <= ((CLICK_GAP_MASK >> CLICK_GAP_OFFSET) - 1) ? f + 1 : 0;   \
                         flagSet1 &= ~CLICK_GAP_MASK;                                 \
                         flagSet1 |= (f << CLICK_GAP_OFFSET);

#define TOGGLE_DISPLAY_STATE \
    (flagSet2 ^= DISPLAY_STATE_MASK)

#define SET_DISPLAY_MODE(x) clicksAndFlags &= ~DISPLAY_MODE_MASK;           \
                            clicksAndFlags |= ((x) << DISPLAY_MODE_OFFSET);

#define NUMBER_OF_MODES 5

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

extern volatile uint8_t flagSet1, flagSet2, flagSet3;
extern volatile uint16_t time;
extern volatile uint16_t timerTime;
extern volatile uint16_t stopwatchTime;
extern volatile int ticks;
