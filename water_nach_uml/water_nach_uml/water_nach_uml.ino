// Pin definitions
const int relayPin = 15;       // Pump relay
const int moisturePin = 14;    // Soil moisture sensor
const int buzzerPin = 8;       // Buzzer for beeping
const int fuelPin = 16;        // Fuel (or water level) sensor

// Threshold values (adjust as needed)
const int moistureThreshold = 500; // Above this value, the soil is considered "dry"
const int fuelThreshold     = 500; // Above this value, the fuel is considered "empty"

// Sleep durations in milliseconds
const unsigned long pumpSleepDuration    = 10 * 60; // Sleep 10 minutes after pumping
const unsigned long nonDrySleepDuration  = 24 * 60 * 60; // Sleep 24 hours if not dry
const unsigned long beepSleepDuration    = 8; // 8 seconds sleep between fuel checks

// This variable is made volatile because it is changed inside
// an interrupt function
volatile int f_wdt=1;

// Watchdog Interrupt Service. This is executed when watchdog timed out.
ISR(WDT_vect) {
	if(f_wdt == 0) {
		// here we can implement a counter the can set the f_wdt to true if
		// the watchdog cycle needs to run longer than the maximum of eight
		// seconds.
		f_wdt=1;
	}
}


void setup() {
  Serial.begin(9600);

  // Set up pin modes
  pinMode(relayPin, INPUT);
  pinMode(moisturePin, INPUT);
  pinMode(fuelPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  pinMode(3,OUTPUT);
  analogWrite(3,255);

  Serial.println("Initialising...");
  setupWatchDogTimer();
  Serial.println("Initialisation complete.");
}

// Function to measure moisture and decide if the soil is dry
bool isDry() {
  int moisture = analogRead(moisturePin);
  Serial.print("Moisture level: ");
  Serial.println(moisture);
  return (moisture > moistureThreshold);
}

// Function to check if the fuel (or water level) is empty.
// (Assumes a higher sensor reading means "empty")
bool fuelIsEmpty() {
  int fuelVal = analogRead(fuelPin);
  Serial.print("Fuel level: ");
  Serial.println(fuelVal);
  return (fuelVal > fuelThreshold);
}

// Function to beep (simulate warning) for fuel empty condition
void beep() {
  tone(buzzerPin, 1000); // 1000 Hz tone
  delay(500);            // Beep for 500ms
  noTone(buzzerPin);
  delay(500);
}

// Function to activate the pump
void pump() {
  Serial.println("Pump active");
  pinMode(relayPin, OUTPUT);
  analogWrite(relayPin, 100); // Relais einschalten
  delay(8000);                   // Pump for 2 seconds (adjust as needed)
  pinMode(relayPin, INPUT);   // Turn off pump
}

void loop() {
  if(f_wdt != 1) {
		return;
	}
  // Toggle the LED on
	Serial.println("not sleep");
	// wait
	delay(20);

	// clear the flag so we can run above code again after the MCU wake up
	f_wdt = 0;

	// Re-enter sleep mode.
	enterSleep();

  // Step 1: Measure moisture
  if (isDry()) {
    // If soil is dry, check fuel repeatedly
    while (fuelIsEmpty()) {
      Serial.println("Fuel is empty, beeping and waiting 8 seconds...");
      beep();
      sleepDuration(beepSleepDuration); // Sleep 8 seconds before checking again
    }
    
    // Once fuel level is acceptable, pump water and then sleep for 10 minutes.
    pump();
    Serial.println("Sleeping for 10 minutes after pumping...");
    sleepDuration(pumpSleepDuration);
    
  } else {
    // Soil moisture is sufficient; sleep for 24 hours.
    Serial.println("Soil is wet. Sleeping for 24 hours...");
    sleepDuration(nonDrySleepDuration);
  }
}
