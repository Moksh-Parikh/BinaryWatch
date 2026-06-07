#pragma once

#include <stdint.h>

void modelUpdate(int8_t clicks, uint8_t clickGap);
void rewriteBufferHours(uint8_t hours);
void rewriteBufferMinutes(uint8_t minutes);
void incrementDisplayMode();
void fillBufferWithTime(uint8_t hours, uint8_t minutes);
void updateTimeBuffer(uint8_t displayMode);
