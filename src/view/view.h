#pragma once
#include <stdint.h>

void fillBufferWithTime(uint8_t hours, uint8_t minutes);
void rewriteBufferHours(uint8_t hours);
void rewriteBufferMinutes(uint8_t minutes);
void updateTimeBuffer(uint8_t displayMode);
