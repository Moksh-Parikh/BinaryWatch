#pragma once

#include <stdint.h>

#define TICKS_PER_SEC_MINUS_1 23
#define GO_TO_SLEEP __asm__ __volatile__ ( "sleep" "\n\t" :: );

void modelUpdate(int8_t clicks);
void incrementDisplayMode();
