#pragma once

// timers TC0 TC1 TC2   channels 0-2 ids 0-2  3-5  6-8     AB 0 1
// use TC1 channel 0 

// TIMER_CLOCK4   84MHz/128 with 16 bit counter give 10 Hz to 656KHz
//  piano 27Hz to 4KHz

extern void startTone(uint32_t pin, uint32_t frequency, int32_t duration);
extern void stopTone(void);
