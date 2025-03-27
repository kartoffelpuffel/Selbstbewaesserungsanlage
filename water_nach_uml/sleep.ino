// This is an example how to put an arduino board into deep sleep to save on battery power and 
// periodically wake up to run some logic. Ideal for sensor stations like plant moisture sensors.
#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

// Blink Before sleeping
#define LED_PIN (13)

// Enters the arduino into sleep mode.
void enterSleep(void)
{
	// There are five different sleep modes in order of power saving:
	// SLEEP_MODE_IDLE - the lowest power saving mode
	// SLEEP_MODE_ADC
	// SLEEP_MODE_PWR_SAVE
	// SLEEP_MODE_STANDBY
	// SLEEP_MODE_PWR_DOWN - the highest power saving mode
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// Now enter sleep mode.
	sleep_mode();

	// The program will continue from here after the WDT timeout

	// First thing to do is disable sleep.
	sleep_disable();

	// Re-enable the peripherals.
	power_all_enable();
}

void sleepFor(uint16_t seconds) {
  for (int i = seconds; i > 0; i--){
    enterSleep();
    seconds -= 8;
  }
  //   if (seconds >= 8) {
  //     setWatchdogFor(8);
  //     enterSleep();
  //     seconds -= 8;
  //   }
  //   else if (seconds >= 4) {
  //     setWatchdogFor(4);
  //     enterSleep();
  //     seconds -= 4;
  //   }
  //   else if (seconds >= 2) {
  //     setWatchdogFor(2);
  //     enterSleep();
  //     seconds -= 2;
  //   }
  //   else { // seconds is 1
  //     setWatchdogFor(1);
  //     enterSleep();
  //     seconds -= 1;
  //   }
  // }
}


void setWatchdogFor(int seconds) {
  // Clear the reset flag
  MCUSR &= ~(1 << WDRF);
  // Set WDCE and WDE to allow changes for the next 4 cycles
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  
  // Configure the watchdog for the desired timeout
  if (seconds == 1) {
    // 1.0 s: WDP3=0, WDP2=1, WDP1=1, WDP0=0
    WDTCSR = (0 << WDP3) | (1 << WDP2) | (1 << WDP1) | (0 << WDP0);
  }
  else if (seconds == 2) {
    // 2.0 s: WDP3=0, WDP2=1, WDP1=1, WDP0=1
    WDTCSR = (0 << WDP3) | (1 << WDP2) | (0 << WDP1) | (1 << WDP0);
  }
  else if (seconds == 4) {
    // 4.0 s: WDP3=1, WDP2=0, WDP1=0, WDP0=0
    WDTCSR = (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (0 << WDP0);
  }
  else if (seconds == 8) {
    // 8.0 s: WDP3=1, WDP2=0, WDP1=0, WDP0=1
    WDTCSR = (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0);
  }
  
  // Enable watchdog interrupt (no reset)
  WDTCSR |= _BV(WDIE);
}
