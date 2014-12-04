//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include "math.h"
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"
#include "Compass.h"
#include "Gyroscope.h"

#include "stm32f3_discovery.h"
// ----------------------------------------------------------------------------
//
// STM32F3 led blink sample (trace via ITM).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// To demonstrate POSIX retargetting, reroute the STDOUT and STDERR to the
// trace device and display messages on both of them.
//
// Then demonstrates how to blink a led with 1Hz, using a
// continuous loop and SysTick delays.
//
// On DEBUG, the uptime in seconds is also displayed on the trace device.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f30x.c
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 2 / 3)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)

// ----- Useful functions -------------------------------------------
#define ABS(x) (x < 0) ? (-x) : x
#define PI (float) 3.14159265f

// Private Variables
__IO uint32_t UserButtonPressed = 0;
__IO uint8_t DataReady = 0;

float MagBuffer[3] = {0.0f};
float AccBuffer[3] = {0.0f};
float fNormAcc = 0x0f;;
float fSinRoll = 0x0f;;
float fCosRoll = 0x0f;;
float fSinPitch = 0x0f;;
float fCosPitch = 0.0f;
float RollAng = 0.0f;
float PitchAng = 0.0f;
float fTiltedX = 0x0f;
float fTiltedY = 0.0f;
__IO float HeadingValue = 0.0f;


float GyroBuffer[3] = {0.0f};
uint8_t Xval = 0x00;
uint8_t Yval = 0x00;




// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main(int argc, char* argv[])
{

  timer_start();

  /* Initialize LEDs */
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  STM_EVAL_LEDInit(LED9);
  STM_EVAL_LEDInit(LED10);

  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);

  /* Initialize UserButton */
  UserButtonPressed = 0;

  while (1) {
    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED5);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED10);

    while (UserButtonPressed == 0x00) {
      STM_EVAL_LEDToggle(LED3);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED5);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED7);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED9);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED10);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED8);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED6);
      timer_sleep(5);
      STM_EVAL_LEDToggle(LED4);
    }

    DataReady = 0x00;

    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED5);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED10);

    GyroConfig();

    while (UserButtonPressed == 0x01) {
      while (DataReady != 0x05) {}
      DataReady = 0x00;

      STM_EVAL_LEDOff(LED3);
      STM_EVAL_LEDOff(LED4);
      STM_EVAL_LEDOff(LED5);
      STM_EVAL_LEDOff(LED6);
      STM_EVAL_LEDOff(LED7);
      STM_EVAL_LEDOff(LED8);
      STM_EVAL_LEDOff(LED9);
      STM_EVAL_LEDOff(LED10);

      GyroReadAngRate(GyroBuffer);
      Xval = ABS((int8_t)(GyroBuffer[0]));
      Yval = ABS((int8_t)(GyroBuffer[1]));

      if (Xval > Yval) {
        if ((int8_t)GyroBuffer[0] > 5.0f) {
          STM_EVAL_LEDOn(LED10);
        }
        if ((int8_t)GyroBuffer[0] < -5.0f) {
          STM_EVAL_LEDOn(LED3);
        }
      } else {
        if ((int8_t)GyroBuffer[1] > 5.0f) {
          STM_EVAL_LEDOn(LED7);
        }
        if ((int8_t)GyroBuffer[1] < -5.0f) {
          STM_EVAL_LEDOn(LED6);
        }
      }
    }

    DataReady = 0x00;

    STM_EVAL_LEDOff(LED3);
    STM_EVAL_LEDOff(LED4);
    STM_EVAL_LEDOff(LED5);
    STM_EVAL_LEDOff(LED6);
    STM_EVAL_LEDOff(LED7);
    STM_EVAL_LEDOff(LED8);
    STM_EVAL_LEDOff(LED9);
    STM_EVAL_LEDOff(LED10);

    CompassConfig();

    while (UserButtonPressed == 0x02) {
      /* Wait for data ready */
      while(DataReady !=0x05) {}
      DataReady = 0x00;

      CompassReadMag(MagBuffer);
      CompassReadAcc(AccBuffer);

      for(uint8_t i=0;i<3;i++)
        AccBuffer[i] /= 100.0f;

      fNormAcc = sqrt((AccBuffer[0]*AccBuffer[0])+(AccBuffer[1]*AccBuffer[1])+(AccBuffer[2]*AccBuffer[2]));

      fSinRoll = -AccBuffer[1]/fNormAcc;
      fCosRoll = sqrt(1.0-(fSinRoll * fSinRoll));
      fSinPitch = AccBuffer[0]/fNormAcc;
      fCosPitch = sqrt(1.0-(fSinPitch * fSinPitch));
      if (fSinRoll > 0) {
        RollAng = (fCosRoll>0) ? acos(fCosRoll)*180/PI : acos(fCosRoll)*180/PI+180;
      } else {
        RollAng = (fCosRoll>0) ? acos(fCosRoll)*180/PI+360 : acos(fCosRoll)*180/PI+180;
      }

      if (fSinPitch > 0) {
        PitchAng = (fCosPitch>0) ? acos(fCosPitch)*180/PI : acos(fCosPitch)*180/PI+180;
      } else {
        PitchAng = (fCosPitch>0) ? acos(fCosPitch)*180/PI+360 : acos(fCosPitch)*180/PI+180;
      }

      if (RollAng >= 360) {
        RollAng = RollAng - 360;
      }

      if (PitchAng >= 360) {
        PitchAng = PitchAng - 360;
      }

      fTiltedX = MagBuffer[0]*fCosPitch+MagBuffer[2]*fSinPitch;
      fTiltedY = MagBuffer[0]*fSinRoll*fSinPitch+MagBuffer[1]*fCosRoll-MagBuffer[1]*fSinRoll*fCosPitch;
      HeadingValue = (float) ((atan2f((float)fTiltedY,(float)fTiltedX))*180)/PI;

      if (HeadingValue < 0)
      {
        HeadingValue = HeadingValue + 360;
      }

      if ((RollAng <= 40.0f) && (PitchAng <= 40.0f))
      {
        if (((HeadingValue < 25.0f)&&(HeadingValue >= 0.0f))||((HeadingValue >=340.0f)&&(HeadingValue <= 360.0f)))
        {
          STM_EVAL_LEDOn(LED10);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED7);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED5);
        }
        else  if ((HeadingValue <70.0f)&&(HeadingValue >= 25.0f))
        {
          STM_EVAL_LEDOn(LED9);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED7);
        }
        else  if ((HeadingValue < 115.0f)&&(HeadingValue >= 70.0f))
        {
          STM_EVAL_LEDOn(LED7);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED5);
        }
        else  if ((HeadingValue <160.0f)&&(HeadingValue >= 115.0f))
        {
          STM_EVAL_LEDOn(LED5);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED7);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED3);
        }
        else  if ((HeadingValue <205.0f)&&(HeadingValue >= 160.0f))
        {
          STM_EVAL_LEDOn(LED3);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED7);
        }
        else  if ((HeadingValue <250.0f)&&(HeadingValue >= 205.0f))
        {
          STM_EVAL_LEDOn(LED4);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED7);
        }
        else  if ((HeadingValue < 295.0f)&&(HeadingValue >= 250.0f))
        {
          STM_EVAL_LEDOn(LED6);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED8);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED5);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED7);
        }
        else  if ((HeadingValue < 340.0f)&&(HeadingValue >= 295.0f))
        {
          STM_EVAL_LEDOn(LED8);
          STM_EVAL_LEDOff(LED6);
          STM_EVAL_LEDOff(LED10);
          STM_EVAL_LEDOff(LED7);
          STM_EVAL_LEDOff(LED9);
          STM_EVAL_LEDOff(LED3);
          STM_EVAL_LEDOff(LED4);
          STM_EVAL_LEDOff(LED5);
        }
      }
      else
      {
        /* Toggle All LEDs */
        STM_EVAL_LEDToggle(LED7);
        STM_EVAL_LEDToggle(LED6);
        STM_EVAL_LEDToggle(LED10);
        STM_EVAL_LEDToggle(LED8);
        STM_EVAL_LEDToggle(LED9);
        STM_EVAL_LEDToggle(LED3);
        STM_EVAL_LEDToggle(LED4);
        STM_EVAL_LEDToggle(LED5);
        /* Delay 50ms */
        timer_sleep(5);
      }
    }
  }

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
