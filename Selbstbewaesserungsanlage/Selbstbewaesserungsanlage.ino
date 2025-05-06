const int moisturePin = 1;
const int relayPin = 9;
const int moistureThreshold = 350;
const int sleepAfterPumpen = 30;
const int sleepAfterMoistureOk = 60;

byte adcsra_save = 135;

// This variable is made volatile because it is changed inside
// an interrupt function
volatile int f_wdt=1;

//debug
int start = 0;

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
  // put your setup code here, to run once:
  Serial.begin(9600);
  setWatchdogFor(1);

  pinMode(moisturePin, INPUT);
	pinMode(relayPin, OUTPUT);
  
}



bool isDry(){
	int moisture = analogRead(moisturePin);
  Serial.print("Moisture level: ");
  Serial.println(moisture);
  Serial.flush();
  return (moisture > moistureThreshold);
}


void pumpen(){
	Serial.println("begin pumpe");
	Serial.flush();

	digitalWrite(relayPin, HIGH);
}



void loop() {
	if(f_wdt != 1) {
		return;
	}

	// clear the flag so we can run above code again after the MCU wake up
	f_wdt = 0;
  

	if(isDry()){
		pumpen();
	}
	else{
		digitalWrite(relayPin, LOW);
		Serial.println("stop pumpe");
		Serial.flush();
		
		Serial.println("gehe schlafen");
		Serial.flush();

		// disable ADC
		ADCSRA = 0;
		sleepFor(4);
		ADCSRA = adcsra_save;
	}
}
