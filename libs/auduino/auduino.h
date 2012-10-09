#ifndef __AUDUINO_H__
#define __AUDUINO_H__

#include <avr/io.h>
#include <avr/interrupt.h>

static uint16_t syncPhaseAcc;
static uint16_t syncPhaseInc;
static uint16_t grainPhaseAcc;
static uint16_t grainPhaseInc;
static uint16_t grainAmp;
static uint8_t grainDecay;
static uint16_t grain2PhaseAcc;
static uint16_t grain2PhaseInc;
static uint16_t grain2Amp;
static uint8_t grain2Decay;

// Map Analogue channels
#define SYNC_CONTROL         (4)
#define GRAIN_FREQ_CONTROL   (0)
#define GRAIN_DECAY_CONTROL  (2)
#define GRAIN2_FREQ_CONTROL  (3)
#define GRAIN2_DECAY_CONTROL (1)


// Changing these will also requires rewriting audioOn()

#if defined(__AVR_ATmega8__)
//
// On old ATmega8 boards.
//    Output is on pin 11
//
#define LED_PIN       13
#define LED_PORT      PORTB
#define LED_BIT       5
#define PWM_PIN       11
#define PWM_VALUE     OCR2
#define PWM_INTERRUPT TIMER2_OVF_vect
#elif defined(__AVR_ATmega1280__)
//
// On the Arduino Mega
//    Output is on pin 3
//
#define LED_PIN       13
#define LED_PORT      PORTB
#define LED_BIT       7
#define PWM_PIN       3
#define PWM_VALUE     OCR3C
#define PWM_INTERRUPT TIMER3_OVF_vect
#else
//
// For modern ATmega168 and ATmega328 boards
//    Output is on pin 3
//
#define PWM_PIN       3
#define PWM_VALUE     OCR2B
#define LED_PIN       13
#define LED_PORT      PORTB
#define LED_BIT       5
#define PWM_INTERRUPT TIMER2_OVF_vect
#endif

void audioOn();

void audioOff();

uint16_t mapPentatonic(uint16_t input);

uint16_t mapPhaseInc(uint16_t input);

#endif // __AUDUINO_H__
