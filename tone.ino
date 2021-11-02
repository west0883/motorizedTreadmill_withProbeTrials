#include <stdint.h>

#include "tone.h"

#define TONE_IRQ    TC3_IRQn

static bool timerConfigured = false;
static bool pinState = false;
static Tc *timer = TC1;
static uint32_t channel = 0;
    
static volatile int32_t toggleCount;
//static uint32_t tonePin;

// timer ISR  TC1 ch 0
void TC3_Handler(void)
{
    TC_GetStatus(TC1, 0);
    
    if (toggleCount != 0)
    {
        pinState = !pinState;
        
        // Toggle pin
        digitalWrite(tonePin, pinState);
        
        if (toggleCount > 0)
        {
            toggleCount--;
        }
    }
    else
    {
        stopTone();
    }
}
    
void startTone(uint32_t pin, uint32_t frequency, int32_t duration)
{
    const uint32_t rc = VARIANT_MCK / 256 / frequency;
    
    tonePin = pin;

    // Disable the interrupt in case we're already running
    NVIC_DisableIRQ(TONE_IRQ);

    // Stop any running timer setup
    TC_Stop(timer, channel);

    // If we are being told to turn off, nothing else to do
    if (duration <= 0)
    {
        return;
    }

    // Calculate how many times to toggle the GPIO for the duration
    toggleCount = 2 * frequency * duration / 1000;
    
    // If we haven't done so yet, configure the timer peripheral
    if (!timerConfigured)
    {
        pmc_set_writeprotect(false);
        
        pmc_enable_periph_clk((uint32_t)TONE_IRQ);
        
        // Counter running up and reset when equals to RC
        TC_Configure(timer, channel,
            TC_CMR_TCCLKS_TIMER_CLOCK4 |
            TC_CMR_WAVE |         // Waveform mode
            TC_CMR_WAVSEL_UP_RC
        );
    
        // RC compare interrupt
        timer->TC_CHANNEL[channel].TC_IER = TC_IER_CPCS;
        timer->TC_CHANNEL[channel].TC_IDR = ~TC_IER_CPCS;
        
        timerConfigured = true;
    }
    
    // Set frequency
    TC_SetRC(timer, channel, rc);

    // Enable the interrupt handling
    NVIC_EnableIRQ(TONE_IRQ);

    // Start running the timer
    TC_Start(timer, channel);
}

void stopTone(void)
{
    // Disable the timer interrupt
    NVIC_DisableIRQ(TONE_IRQ);
    
    // Stop timer
    TC_Stop(timer, channel);
    
    // No signal on pin
    digitalWrite(tonePin, LOW);
}
