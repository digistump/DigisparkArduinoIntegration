/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.c 970 2010-05-25 20:16:15Z dmellis $

  Modified 28-08-2009 for attiny84 R.Wiersma
  Modified 14-10-2009 for attiny45 Saposoft
  Modified 20-11-2010 - B.Cook - Rewritten to use the various Veneers.
*/

#include "core_build_options.h"
#include "core_adc.h"
#include "core_timers.h"
#include "wiring_private.h"
#include "ToneTimer.h"

#define millistimer_(t)                           TIMER_PASTE_A( timer, TIMER_TO_USE_FOR_MILLIS, t )
#define MillisTimer_(f)                           TIMER_PASTE_A( Timer, TIMER_TO_USE_FOR_MILLIS, f )
#define MILLISTIMER_(c)                           TIMER_PASTE_A( TIMER, TIMER_TO_USE_FOR_MILLIS, c )

#define MillisTimer_SetToPowerup                  MillisTimer_(SetToPowerup)
#define MillisTimer_SetWaveformGenerationMode     MillisTimer_(SetWaveformGenerationMode)
#define MillisTimer_GetCount                      MillisTimer_(GetCount)
#define MillisTimer_IsOverflowSet                 MillisTimer_(IsOverflowSet)
#define MillisTimer_ClockSelect                   MillisTimer_(ClockSelect)
#define MillisTimer_EnableOverflowInterrupt       MillisTimer_(EnableOverflowInterrupt)
#define MILLISTIMER_OVF_vect                      MILLISTIMER_(OVF_vect)


#if F_CPU >= 3000000L
  #define MillisTimer_Prescale_Index  MillisTimer_(Prescale_Value_64)
  #define MillisTimer_Prescale_Value  (64)
  #define ToneTimer_Prescale_Index    ToneTimer_(Prescale_Value_64)
  #define ToneTimer_Prescale_Value    (64)
#else
  #define MillisTimer_Prescale_Index  MillisTimer_(Prescale_Value_8)
  #define MillisTimer_Prescale_Value  (8)
  #define ToneTimer_Prescale_Index    ToneTimer_(Prescale_Value_8)
  #define ToneTimer_Prescale_Value    (8)
#endif

// the prescaler is set so that the millis timer ticks every MillisTimer_Prescale_Value (64) clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_MILLIS_OVERFLOW (clockCyclesToMicroseconds(MillisTimer_Prescale_Value * 256))

// the whole number of milliseconds per millis timer overflow
#define MILLIS_INC (MICROSECONDS_PER_MILLIS_OVERFLOW / 1000)

// the fractional number of milliseconds per millis timer overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_MILLIS_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long millis_timer_overflow_count = 0;
volatile unsigned long millis_timer_millis = 0;
static unsigned char millis_timer_fract = 0;

ISR(MILLISTIMER_OVF_vect)
{
  // copy these to local variables so they can be stored in registers
  // (volatile variables must be read from memory on every access)
  unsigned long m = millis_timer_millis;
  unsigned char f = millis_timer_fract;

/* rmv: The code below generates considerably less code (emtpy Sketch is 326 versus 304)...

  m += MILLIS_INC;
  f += FRACT_INC;
  if (f >= FRACT_MAX) {
    f -= FRACT_MAX;
    m += 1;
  }
...rmv */

  f += FRACT_INC;

  if (f >= FRACT_MAX) 
  {
    f -= FRACT_MAX;
    m = m + MILLIS_INC + 1;
  }
  else
  {
    m += MILLIS_INC;
  }

  millis_timer_fract = f;
  millis_timer_millis = m;
  millis_timer_overflow_count++;
}

unsigned long millis()
{
  unsigned long m;
  uint8_t oldSREG = SREG;

  // disable interrupts while we read millis_timer_millis or we might get an
  // inconsistent value (e.g. in the middle of a write to millis_timer_millis)
  cli();
  m = millis_timer_millis;
  SREG = oldSREG;

  return m;
}

unsigned long micros() 
{
  unsigned long m;
  uint8_t oldSREG = SREG, t;
  
  cli();
  m = millis_timer_overflow_count;
  t = MillisTimer_GetCount();
  
  if (MillisTimer_IsOverflowSet() && (t < 255))
    m++;

  SREG = oldSREG;
  
  return ((m << 8) + t) * (MillisTimer_Prescale_Value / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms)
{
  uint16_t start = (uint16_t)micros();

  while (ms > 0) {
    if (((uint16_t)micros() - start) >= 1000) {
      ms--;
      start += 1000;
    }
  }
}

/* Delay for the given number of microseconds.  Assumes a 8 or 16 MHz clock. */
void delayMicroseconds(unsigned int us)
{
  // calling avrlib's delay_us() function with low values (e.g. 1 or
  // 2 microseconds) gives delays longer than desired.
  //delay_us(us);

#if F_CPU >= 16000000L
  // for the 16 MHz clock on most Arduino boards

  // for a one-microsecond delay, simply return.  the overhead
  // of the function call yields a delay of approximately 1 1/8 us.
  if (--us == 0)
    return;

  // the following loop takes a quarter of a microsecond (4 cycles)
  // per iteration, so execute it four times for each microsecond of
  // delay requested.
  us <<= 2;

  // account for the time taken in the preceeding commands.
  us -= 2;
#else
  // for the 8 MHz internal clock on the ATmega168

  // for a one- or two-microsecond delay, simply return.  the overhead of
  // the function calls takes more than two microseconds.  can't just
  // subtract two, since us is unsigned; we'd overflow.
  if (--us == 0)
    return;
  if (--us == 0)
    return;

  // the following loop takes half of a microsecond (4 cycles)
  // per iteration, so execute it twice for each microsecond of
  // delay requested.
  us <<= 1;
    
  // partially compensate for the time taken by the preceeding commands.
  // we can't subtract any more than this or we'd overflow w/ small delays.
  us--;
#endif

  // busy wait
  __asm__ __volatile__ (
    "1: sbiw %0,1" "\n\t" // 2 cycles
    "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
  );
}

static void initToneTimerInternal(void)
{
  // Stop the clock while we make changes
  ToneTimer_ClockSelect( ToneTimer_(Stopped) );

  // Set the timer to phase-correct PWM
  #if defined( TONETIMER_SUPPORTS_PHASE_CORRECT_PWM ) && TONETIMER_SUPPORTS_PHASE_CORRECT_PWM
    ToneTimer_SetWaveformGenerationMode( ToneTimer_(Phase_Correct_PWM_FF) );
  #else
    ToneTimer_SetWaveformGenerationMode( ToneTimer_(Fast_PWM_FF) );
  #endif

  // Timer is processor clock divided by ToneTimer_Prescale_Index (64)
  ToneTimer_ClockSelect( ToneTimer_Prescale_Index );
}

void initToneTimer(void)
{
  // Ensure the timer is in the same state as power-up
  ToneTimer_SetToPowerup();

  #if defined( INITIALIZE_SECONDARY_TIMERS ) && INITIALIZE_SECONDARY_TIMERS
    // Prepare the timer for PWM
    initToneTimerInternal();
  #endif
}

void init(void)
{
  // this needs to be called before setup() or some functions won't work there
  sei();

  // In case the bootloader left our millis timer in a bad way
  #if defined( HAVE_BOOTLOADER ) && HAVE_BOOTLOADER
    MillisTimer_SetToPowerup();
  #endif

  // Use the Millis Timer for fast PWM
  MillisTimer_SetWaveformGenerationMode( MillisTimer_(Fast_PWM_FF) );

  // Millis timer is always processor clock divided by MillisTimer_Prescale_Value (64)
  MillisTimer_ClockSelect( MillisTimer_Prescale_Index );

  // Enable the overlow interrupt (this is the basic system tic-toc for millis)
  MillisTimer_EnableOverflowInterrupt();

  // Initialize the timer used for Tone
  #if defined( INITIALIZE_SECONDARY_TIMERS ) && INITIALIZE_SECONDARY_TIMERS
    initToneTimerInternal();
  #endif

  // Initialize the ADC
  #if defined( INITIALIZE_ANALOG_TO_DIGITAL_CONVERTER ) && INITIALIZE_ANALOG_TO_DIGITAL_CONVERTER
    ADC_PrescalerSelect( ADC_ARDUINO_PRESCALER );
    ADC_Enable();
  #endif
}

